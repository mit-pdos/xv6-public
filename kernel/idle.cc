#include "types.h"
#include "kernel.hh"
#include "amd64.h"
#include "spinlock.h"
#include "condvar.h"
#include "proc.hh"
#include "cpu.hh"
#include "percpu.hh"
#include "wq.hh"
#include "uwq.hh"
#include "kmtrace.hh"
#include "bits.hh"

struct idle {
  struct proc *cur;
  struct proc *heir;
  SLIST_HEAD(zombies, proc) zombies;
  struct spinlock lock;
};

static percpu<idle> idlem;

void idleloop(void);

struct proc *
idleproc(void)
{
  return idlem->cur;
}

void
idlezombie(struct proc *p)
{
  acquire(&idlem[mycpu()->id].lock);
  SLIST_INSERT_HEAD(&idlem[mycpu()->id].zombies, p, child_next);
  release(&idlem[mycpu()->id].lock);
}

void
idlebequeath(void)
{
  // Only the current idle thread may call this function

  assert(mycpu()->ncli > 0);
  assert(myproc() == idlem->cur);

  assert(idlem->heir != nullptr);

  idlem->cur = idlem->heir;
  acquire(&idlem->heir->lock);
  idlem->heir->set_state(RUNNABLE);
  release(&idlem->heir->lock);
}

void
idleheir(void *x)
{
  post_swtch();

  idlem->heir = nullptr;
  idleloop();
}

static inline void
finishzombies(void)
{
  struct idle *i = &idlem[mycpu()->id];

  if (!SLIST_EMPTY(&i->zombies)) {
    struct proc *p, *np;
    acquire(&i->lock);
    SLIST_FOREACH_SAFE(p, &i->zombies, child_next, np) {
      SLIST_REMOVE(&i->zombies, p, proc, child_next);
      finishproc(p);
    }
    release(&i->lock);
  }
}

void
idleloop(void)
{
  // Test the work queue
  //extern void testwq(void);
  //testwq();

  //extern void benchwq(void);
  //benchwq();

  // Enabling mtrace calls in scheduler generates many mtrace_call_entrys.
  // mtrace_call_set(1, cpu->id);
  mtstart(idleloop, myproc());

  // The scheduler ensures that each idle loop always runs on the same CPU
  struct idle *myidle = idlem.get_unchecked();

  sti();
  for (;;) {
    acquire(&myproc()->lock);
    myproc()->set_state(RUNNABLE);
    sched();

    finishzombies();

    if (steal() == 0) {
      int worked;
      do {
        assert(mycpu()->ncli == 0);

        // If we don't have an heir, try to allocate one
        if (myidle->heir == nullptr) {
          struct proc *p;
          p = proc::alloc();
          if (p == nullptr)
            break;
          snprintf(p->name, sizeof(p->name), "idle_%u", mycpu()->id);          
          p->cpuid = mycpu()->id;
          p->cpu_pin = 1;
          p->context->rip = (u64)idleheir;
          p->cwd = nullptr;
          myidle->heir = p;
        }

        if (uwq_trywork())
          break;

        worked = wq_trywork();
        // If we are no longer the idle thread, exit
        if (worked && myidle->cur != myproc())
          exit();
      } while(worked);
      sti();
      nop_pause();
    }
  }
}

void
initidle(void)
{
  struct proc *p = proc::alloc();
  if (!p)
    panic("initidle proc::alloc");

  if (myid() == mpbcpu()) {
    u32 eax, ebx, ecx;
    cpuid(CPUID_FEATURES, nullptr, nullptr, &ecx, nullptr);
    if (ecx & FEATURE_ECX_MWAIT) {
      // Check smallest and largest line sizes
      cpuid(CPUID_MWAIT, &eax, &ebx, nullptr, nullptr);
      assert((u16)eax == 0x40);
      assert((u16)ebx == 0x40);
    }
  }

  SLIST_INIT(&idlem->zombies);
  initlock(&idlem->lock, "idle_lock", LOCKSTAT_IDLE);

  snprintf(p->name, sizeof(p->name), "idle_%u", myid());
  mycpu()->proc = p;
  myproc()->cpuid = myid();
  myproc()->cpu_pin = 1;
  idlem->cur = p;
}
