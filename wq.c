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

#define NSLOTS (1 << WQSHIFT)

struct wqueue {
  struct wqthread *thread[NSLOTS];
  volatile int head __mpalign__;

  struct spinlock lock;
  volatile int tail;
  __padout__;
} __mpalign__;

struct wqthread {
  u64 rip;
  u64 arg0;
  u64 arg1;
  volatile u64 *ref;  // pointer to parent wqframe.ref
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
  return &myproc()->wqframe;
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
  int i;

  acquire(&q->lock);
  i = q->head;
  if ((i - q->tail) == 0) {
    release(&q->lock);
    return NULL;
  }
  i = (i-1) & (NSLOTS-1);
  q->head--;
  release(&q->lock);

  wq_stat()->pop++;
  return q->thread[i];
}

static struct wqthread *
__wq_steal(struct wqueue *q)
{
  int i;

  acquire(&q->lock);
  i = q->tail;
  if ((i - q->head) == 0) {
    release(&q->lock);
    return NULL;
  }
  i = i & (NSLOTS-1);
  q->tail++;
  release(&q->lock);

  wq_stat()->steal++;
  return q->thread[i];
}

static void
__wq_run(struct wqthread *th)
{
  void (*fn)(uptr arg0, uptr arg1) = (void*)th->rip;
  fn(th->arg0, th->arg1);
  subfetch(th->ref, 1);
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
  th->ref = &wq_frame()->ref;

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

void
wq_start(void)
{ 
  pushcli();
  if (myproc()->wqframe.ref != 0)
    panic("wq_start");
}

void
wq_end(void)
{
  while (myproc()->wqframe.ref != 0) {
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

  for (i = 0; i < NCPU; i++) {
    initlock(&queue[i].lock, "queue lock");
  }
}
#endif // WQENABLE
