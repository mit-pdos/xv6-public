#include "types.h"
#include "kernel.hh"
#include "amd64.h"
#include "spinlock.h"
#include "condvar.h"
#include "proc.hh"
#include "cpu.hh"
#include "sched.hh"
#include "percpu.hh"

// XXX(sbw) these should be padded out
struct idle : public pad {
  struct proc *cur;
  struct proc *heir;
  SLIST_HEAD(zombies, proc) zombies;
  struct spinlock lock;
};

static percpu<idle> idlem;

struct proc * idlep[NCPU] __mpalign__;

struct heir {
  struct proc *proc;
  __padout__;
} __mpalign__;

struct heir heir[NCPU] __mpalign__;

void idleloop(void);

struct proc *
idleproc(void)
{
  assert(mycpu()->ncli > 0);
  return idlep[mycpu()->id];
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
  struct heir *h;

  assert(mycpu()->ncli > 0);
  assert(myproc() == idlep[mycpu()->id]);

  h = &heir[mycpu()->id];
  assert(h->proc != nullptr);

  idlep[mycpu()->id] = h->proc;
  acquire(&h->proc->lock);
  h->proc->set_state(RUNNABLE);
  release(&h->proc->lock);
}


static void
idleheir(void *x)
{
  post_swtch();

  heir[mycpu()->id].proc = nullptr;
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
  struct heir *h;

  h = &heir[mycpu()->id];

  // Test the work queue
  //extern void testwq(void);
  //testwq();

  //extern void benchwq(void);
  //benchwq();

  // Enabling mtrace calls in scheduler generates many mtrace_call_entrys.
  // mtrace_call_set(1, cpu->id);
  //mtstart(scheduler, idlep);

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
        if (h->proc == nullptr) {
          struct proc *p;
          p = allocproc();
          if (p == nullptr)
            break;
          snprintf(p->name, sizeof(p->name), "idleh_%u", mycpu()->id);          
          p->cpuid = mycpu()->id;
          p->cpu_pin = 1;
          p->context->rip = (u64)idleheir;
          p->cwd = nullptr;
          h->proc = p;
        }

        worked = wq_trywork();
        // If we are no longer the idle thread, exit
        if (worked && idlep[mycpu()->id] != myproc())
          exit();
      } while(worked);
      sti();
    }
  }
}

void
initidle(void)
{
  struct proc *p = allocproc();
  if (!p)
    panic("initidle allocproc");

  SLIST_INIT(&idlem[cpunum()].zombies);
  initlock(&idlem[cpunum()].lock, "idle_lock", LOCKSTAT_IDLE);

  snprintf(p->name, sizeof(p->name), "idle_%u", cpunum());
  mycpu()->proc = p;
  myproc()->cpuid = cpunum();
  myproc()->cpu_pin = 1;
  idlep[cpunum()] = p;
}
