// cilk style run queue built on wq.cc:
//   A core pushes work to the head of its per-core wq.
//   A core pops work from the head of its per-core wq.
//   A core pops work from the tail of another core's per-core wqueue.
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
#include "wq.hh"
#include "percpu.hh"

struct cilkstat {
  u64 push;
  u64 full;
  u64 steal;
};
static percpu<cilkstat> stat;

static struct cilkframe *
cilk_frame(void)
{
  return mycpu()->cilkframe;
}

static void
__cilk_run(struct work *w, void *xfn, void *arg0, void *arg1, void *xframe)
{
  void (*fn)(uptr arg0, uptr arg1) = (void(*)(uptr,uptr))xfn;
  struct cilkframe *frame = (struct cilkframe *)xframe;
  struct cilkframe *old = mycpu()->cilkframe;

  if (old != frame)
    stat->steal++;

  mycpu()->cilkframe = frame;
  fn((uptr)arg0, (uptr)arg1);
  mycpu()->cilkframe = old;
  frame->ref--;
}

// Add the (rip, arg0, arg1) work to the local work queue.
// Guarantees some core will at some point execute the work.
// The current core might execute the work immediately.
void
cilk_push(void (*fn)(uptr, uptr), u64 arg0, u64 arg1)
{
  struct work *w;

  w = allocwork();
  if (w == nullptr) {
    fn(arg0, arg1);
    return;
  }
  w->rip = (void*)__cilk_run;
  w->arg0 = (void*)fn;
  w->arg1 = (void*)arg0;
  w->arg2 = (void*)arg1;
  w->arg3 = (void*)cilk_frame();

  if (wq_push(w)) {
    freework(w);
    fn(arg0, arg1);
    stat->full++;
  } else {
    cilk_frame()->ref++;
    stat->push++;
  }
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
  while (cilk_frame()->ref != 0)
    wq_trywork();

  mycpu()->cilkframe = 0;
  popcli();
}

void
cilk_dump(void)
{
  int i;
  for (i = 0; i < NCPU; i++)
    cprintf("push %lu full %lu steal %lu\n",
            stat[i].push, stat[i].full, stat[i].steal);
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
      cilk_push(__test_stub, i, i);
    cilk_end();
    e = rdtsc();
    cprintf("testcilk: %lu\n", (e-s)/iters);
    cilk_dump();
    running = 0;
  } else {
    while (running)
      wq_trywork();
  }
  popcli();
}

void
initcilkframe(struct cilkframe *cilk)
{
  memset(cilk, 0, sizeof(*cilk));
}
#endif // CILKENABLE
