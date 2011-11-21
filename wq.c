// cilk style run queue
// A work queue is built from NCPU per-core wqueues.
// A core pushes work to the head of its per-core wqueue.
// A core pops work from the head of its per-core wqueue.
// A core pops work from the tail of another core's per-core wqueue.
//
// Usage:
//   void goo(uptr a0, uptr a1) {
//     char *arg = (char*) a0;
//     cprintf("goo\n");
//     arg[1] = 'g';
//   }
//   void foo(uptr a0, uptr a1) {
//     char *arg = (char*) a0;
//     wq_push(goo, a0, 0);
//     arg[0] = 'f';
//     cprintf("foo\n");
//   }
//   void example(void) {
//     char arg[2];
//     wq_start();
//     wq_push(foo, (uptr)arg, 0);
//     cprintf("example\n");     
//     wq_end();
//     cprintf("%c %c\n", arg[0], arg[1]);
//   }

#if WQENABLE
#include "types.h"
#include "kernel.h"
#include "amd64.h"
#include "cpu.h"
#include "bits.h"
#include "spinlock.h"
#include "condvar.h"
#include "queue.h"
#include "proc.h"
#include "mtrace.h"
#include "qlock.h"

#define NSLOTS (1 << WQSHIFT)

struct wqueue {
  struct wqthread *thread[NSLOTS];
  volatile int head __mpalign__;

  qlock_t lock;
  volatile int tail;
  __padout__;
} __mpalign__;

struct wqthread {
  u64 rip;
  u64 arg0;
  u64 arg1;
  struct wqframe *frame;  // parent wqframe
  __padout__;
} __mpalign__;

struct wqstat {
  u64 push;
  u64 full;
  u64 pop;
  u64 steal;
  __padout__;
} __mpalign__;

struct wqueue queue[NCPU] __mpalign__;
struct wqstat stat[NCPU] __mpalign__;

static struct wqueue *
wq_cur(void)
{
  return &queue[mycpu()->id];
}

static struct wqframe *
wq_frame(void)
{
  return mycpu()->wqframe;
}

static struct wqstat *
wq_stat(void)
{
  return &stat[mycpu()->id];
}

static int
__wq_push(struct wqueue *q, struct wqthread *t)
{
  int i;

  i = q->head;
  if ((i - q->tail) == NSLOTS) {
    wq_stat()->full++;
    return -1;
  }
  i = i & (NSLOTS-1);
  q->thread[i] = t;
  q->head++;

  wq_stat()->push++;
  return 0;
}

static struct wqthread *
__wq_pop(struct wqueue *q)
{
  struct qnode qn;
  int i;

  ql_lock(&q->lock, &qn);
  i = q->head;
  if ((i - q->tail) == 0) {
    ql_unlock(&q->lock, &qn);
    return NULL;
  }
  i = (i-1) & (NSLOTS-1);
  q->head--;
  ql_unlock(&q->lock, &qn);

  wq_stat()->pop++;
  return q->thread[i];
}

static struct wqthread *
__wq_steal(struct wqueue *q)
{
  struct qnode qn;
  int i;

  ql_lock(&q->lock, &qn);
  i = q->tail;
  if ((i - q->head) == 0) {
    ql_unlock(&q->lock, &qn);
    return NULL;
  }
  i = i & (NSLOTS-1);
  q->tail++;
  ql_unlock(&q->lock, &qn);

  wq_stat()->steal++;
  return q->thread[i];
}

static void
__wq_run(struct wqthread *th)
{
  void (*fn)(uptr arg0, uptr arg1) = (void*)th->rip;
  struct wqframe *old = mycpu()->wqframe;

  mycpu()->wqframe = th->frame;
  fn(th->arg0, th->arg1);
  mycpu()->wqframe = old;
  subfetch(&th->frame->ref, 1);
  kfree(th);
}

// Add the (rip, arg0, arg1) work to the local work queue.
// Guarantees some core will at some point execute the work.
// The current core might execute the work immediately.
void
wq_push(void *rip, u64 arg0, u64 arg1)
{
  void (*fn)(uptr, uptr) = rip;
  struct wqthread *th;

  th = (struct wqthread *) kalloc();
  if (th == NULL) {
    fn(arg0, arg1);
    return;
  }
  th->rip = (uptr) rip;
  th->arg0 = arg0;
  th->arg1 = arg1;
  th->frame = wq_frame();

  if (__wq_push(wq_cur(), th)) {
    kfree(th);
    fn(arg0, arg1);
  } else 
    fetchadd(&wq_frame()->ref, 1);
}

// Try to execute one wqthread.
// Check local queue then steal from other queues. 
int
wq_trywork(void)
{
  struct wqthread *th;
  int i;

  pushcli();
  th = __wq_pop(wq_cur());
  if (th != NULL) {
    __wq_run(th);
    popcli();
    return 1;
  }

  // XXX(sbw) should be random
  for (i = 0; i < NCPU; i++) {
    if (i == mycpu()->id)
        continue;
    
    th = __wq_steal(&queue[i]);
    if (th != NULL) {
      __wq_run(th);
      popcli();
      return 1;
    }
  }

  popcli();
  return 0;
}

// Start a new work queue frame.
// We don't allow nested work queue frames.
void
wq_start(void)
{ 
  pushcli();
  if (myproc()->wqframe.ref != 0)
    panic("wq_start");
  mycpu()->wqframe = &myproc()->wqframe;
}

// End of the current work queue frame.
// The core works while the reference count of the current
// work queue frame is not 0.
void
wq_end(void)
{
  while (wq_frame()->ref != 0) {
    struct wqthread *th;
    int i;

    while ((th = __wq_pop(wq_cur())) != NULL)
      __wq_run(th);

    for (i = 0; i < NCPU; i++) {
      th = __wq_steal(&queue[i]);
      if (th != NULL) {
        __wq_run(th);
        break;
      }
    }
  }
  mycpu()->wqframe = NULL;
  popcli();
}

void
wq_dump(void)
{
  int i;
  for (i = 0; i < NCPU; i++)
    cprintf("push %lu full %lu pop %lu steal %lu\n",
            stat[i].push, stat[i].full, stat[i].pop, stat[i].steal);
}

static void
__test_stub(uptr a0, uptr a1)
{
  //cprintf("%lu, %lu\n", a0, a1);
}

void
testwq(void)
{
  enum { iters = 1000 };
  static volatile int running = 1;
  u64 e, s;
  int i;
  
  pushcli();
  if (mycpu()->id == 0) {
    microdelay(1);
    s = rdtsc();
    wq_start();
    for (i = 0; i < iters; i++)
      wq_push(__test_stub, i, i);
    wq_end();
    e = rdtsc();
    cprintf("testwq: %lu\n", (e-s)/iters);
    wq_dump();
    running = 0;
  } else {
    while (running)
      wq_trywork();
  }
  popcli();
}

void
initwqframe(struct wqframe *wq)
{
  memset(wq, 0, sizeof(*wq));
}

void
initwq(void)
{
  int i;

  for (i = 0; i < NCPU; i++)
    ql_init(&queue[i].lock, "queue lock");
}
#endif // WQENABLE
