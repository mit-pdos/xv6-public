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
//     cilk_push(goo, a0, 0);
//     arg[0] = 'f';
//     cprintf("foo\n");
//   }
//   void example(void) {
//     char arg[2];
//     cilk_start();
//     cilk_push(foo, (uptr)arg, 0);
//     cprintf("example\n");     
//     cilk_end();
//     cprintf("%c %c\n", arg[0], arg[1]);
//   }

#if CILKENABLE
#include "types.h"
#include "kernel.hh"
#include "amd64.h"
#include "cpu.hh"
#include "bits.hh"
#include "spinlock.h"
#include "condvar.h"
#include "queue.h"
#include "proc.hh"
#include "mtrace.h"
#include "qlock.h"

#define NSLOTS (1 << CILKSHIFT)

struct cilkqueue {
  struct cilkthread *thread[NSLOTS];
  volatile int head __mpalign__;

  qlock_t lock;
  volatile int tail;
  __padout__;
} __mpalign__;

struct cilkthread {
  u64 rip;
  u64 arg0;
  u64 arg1;
  struct cilkframe *frame;  // parent cilkframe
  __padout__;
} __mpalign__;

struct cilkstat {
  u64 push;
  u64 full;
  u64 pop;
  u64 steal;
  __padout__;
} __mpalign__;

struct cilkqueue queue[NCPU] __mpalign__;
struct cilkstat stat[NCPU] __mpalign__;

static struct cilkqueue *
cilk_cur(void)
{
  return &queue[mycpu()->id];
}

static struct cilkframe *
cilk_frame(void)
{
  return mycpu()->cilkframe;
}

static struct cilkstat *
cilk_stat(void)
{
  return &stat[mycpu()->id];
}

static int
__cilk_push(struct cilkqueue *q, struct cilkthread *t)
{
  int i;

  i = q->head;
  if ((i - q->tail) == NSLOTS) {
    cilk_stat()->full++;
    return -1;
  }
  i = i & (NSLOTS-1);
  q->thread[i] = t;
  q->head++;

  cilk_stat()->push++;
  return 0;
}

static struct cilkthread *
__cilk_pop(struct cilkqueue *q)
{
  struct qnode qn;
  int i;

  ql_lock(&q->lock, &qn);
  i = q->head;
  if ((i - q->tail) == 0) {
    ql_unlock(&q->lock, &qn);
    return 0;
  }
  i = (i-1) & (NSLOTS-1);
  q->head--;
  ql_unlock(&q->lock, &qn);

  cilk_stat()->pop++;
  return q->thread[i];
}

static struct cilkthread *
__cilk_steal(struct cilkqueue *q)
{
  struct qnode qn;
  int i;

  ql_lock(&q->lock, &qn);
  i = q->tail;
  if ((i - q->head) == 0) {
    ql_unlock(&q->lock, &qn);
    return 0;
  }
  i = i & (NSLOTS-1);
  q->tail++;
  ql_unlock(&q->lock, &qn);

  cilk_stat()->steal++;
  return q->thread[i];
}

static void
__cilk_run(struct cilkthread *th)
{
  void (*fn)(uptr arg0, uptr arg1) = (void(*)(uptr,uptr))th->rip;
  struct cilkframe *old = mycpu()->cilkframe;

  mycpu()->cilkframe = th->frame;
  fn(th->arg0, th->arg1);
  mycpu()->cilkframe = old;
  th->frame->ref--;
  kfree(th);
}

// Add the (rip, arg0, arg1) work to the local work queue.
// Guarantees some core will at some point execute the work.
// The current core might execute the work immediately.
void
cilk_push(void *rip, u64 arg0, u64 arg1)
{
  void (*fn)(uptr, uptr) = (void(*)(uptr,uptr))rip;
  struct cilkthread *th;

  th = (struct cilkthread *) kalloc();
  if (th == NULL) {
    fn(arg0, arg1);
    return;
  }
  th->rip = (uptr) rip;
  th->arg0 = arg0;
  th->arg1 = arg1;
  th->frame = cilk_frame();

  if (__cilk_push(cilk_cur(), th)) {
    kfree(th);
    fn(arg0, arg1);
  } else
    cilk_frame()->ref++;
}

// Try to execute one cilkthread.
// Check local queue then steal from other queues. 
int
cilk_trywork(void)
{
  struct cilkthread *th;
  int i;

  pushcli();
  th = __cilk_pop(cilk_cur());
  if (th != NULL) {
    __cilk_run(th);
    popcli();
    return 1;
  }

  // XXX(sbw) should be random
  for (i = 0; i < NCPU; i++) {
    if (i == mycpu()->id)
        continue;
    
    th = __cilk_steal(&queue[i]);
    if (th != NULL) {
      __cilk_run(th);
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
cilk_start(void)
{ 
  pushcli();
  if (myproc()->cilkframe.ref != 0)
    panic("cilk_start");
  mycpu()->cilkframe = &myproc()->cilkframe;
}

// End of the current work queue frame.
// The core works while the reference count of the current
// work queue frame is not 0.
void
cilk_end(void)
{
  while (cilk_frame()->ref != 0) {
    struct cilkthread *th;
    int i;

    while ((th = __cilk_pop(cilk_cur())) != NULL)
      __cilk_run(th);

    for (i = 0; i < NCPU; i++) {
      th = __cilk_steal(&queue[i]);
      if (th != NULL) {
        __cilk_run(th);
        break;
      }
    }
  }
  mycpu()->cilkframe = 0;
  popcli();
}

void
cilk_dump(void)
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
testcilk(void)
{
  enum { iters = 1000 };
  static volatile int running = 1;
  u64 e, s;
  int i;
  
  pushcli();
  if (mycpu()->id == 0) {
    microdelay(1);
    s = rdtsc();
    cilk_start();
    for (i = 0; i < iters; i++)
      cilk_push((void*) __test_stub, i, i);
    cilk_end();
    e = rdtsc();
    cprintf("testcilk: %lu\n", (e-s)/iters);
    cilk_dump();
    running = 0;
  } else {
    while (running)
      cilk_trywork();
  }
  popcli();
}

void
initcilkframe(struct cilkframe *cilk)
{
  memset(cilk, 0, sizeof(*cilk));
}

void
initcilk(void)
{
  int i;

  for (i = 0; i < NCPU; i++)
    ql_init(&queue[i].lock, "queue lock");
}
#endif // CILKENABLE
