#include "types.h"
#include "kernel.hh"
#include "mmu.h"
#include "amd64.h"
#include "spinlock.h"
#include "condvar.h"
#include "queue.h"
#include "proc.hh"
#include "cpu.hh"
#include "bits.hh"
#include "kmtrace.hh"
#include "sched.hh"
#include "vm.hh"

enum { sched_debug = 0 };

struct runq {
  STAILQ_HEAD(queue, proc) q;
  struct spinlock lock;
  __padout__;
};

static struct runq runq[NCPU] __mpalign__;

void
post_swtch(void)
{
  if (mycpu()->prev->get_state() == RUNNABLE && 
      mycpu()->prev != idleproc())
    addrun(mycpu()->prev);
  release(&mycpu()->prev->lock);
}

void
sched(void)
{
  extern void threadstub(void);
  extern void forkret(void);
  int intena;

#if SPINLOCK_DEBUG
  if(!holding(&myproc()->lock))
    panic("sched proc->lock");
#endif
  if (myproc() == idleproc() && 
      myproc()->get_state() != RUNNABLE) {
    extern void idlebequeath(void);
    idlebequeath();
  }
  if(mycpu()->ncli != 1)
    panic("sched locks");
  if(myproc()->get_state() == RUNNING)
    panic("sched running");
  if(readrflags()&FL_IF)
    panic("sched interruptible");
  intena = mycpu()->intena;
  myproc()->curcycles += rdtsc() - myproc()->tsc;
  if (myproc()->get_state() == ZOMBIE)
    mtstop(myproc());
  else
    mtpause(myproc());
  mtign();

  struct proc *next = schednext();
  if (next == nullptr) {
    if (myproc()->get_state() != RUNNABLE) {
      next = idleproc();
    } else {
      myproc()->set_state(RUNNING);
      mycpu()->intena = intena;
      release(&myproc()->lock);
      return;
    }
  }

  if (next->get_state() != RUNNABLE)
    panic("non-RUNNABLE next %s %u", next->name, next->get_state());

  struct proc *prev = myproc();
  mycpu()->proc = next;
  mycpu()->prev = prev;

  switchvm(next);
  next->set_state(RUNNING);
  next->tsc = rdtsc();

  if (next->context->rip != (uptr)forkret && 
      next->context->rip != (uptr)threadstub)
  {
    mtresume(next);
  }
  mtrec();

  swtch(&prev->context, next->context);
  mycpu()->intena = intena;
  post_swtch();
}

void
addrun(struct proc *p)
{
  // Always called with p->lock held
  struct runq *q;

  p->set_state(RUNNABLE);

  q = &runq[p->cpuid];
  acquire(&q->lock);
  STAILQ_INSERT_HEAD(&q->q, p, runqlink);
  p->runq = q;
  release(&q->lock);
}

void
delrun(struct proc *p)
{
  // Always called with p->lock held
  struct runq *q;

  q = p->runq;
  acquire(&q->lock);
  STAILQ_REMOVE(&q->q, p, proc, runqlink);
  release(&q->lock);
}

int
steal(void)
{
  struct proc *steal;
  int i;
  int r = 0;

  pushcli();
  
  for (i = 1; i < ncpu; i++) {
    struct runq *q = &runq[(i+mycpu()->id) % ncpu];
    struct proc *p;

    // XXX(sbw) Look for a process to steal.  Acquiring q->lock
    // then p->lock can result in deadlock.  So we acquire
    // q->lock, scan for a process, drop q->lock, acquire p->lock,
    // and then check that it's still ok to steal p.
    steal = nullptr;
    if (tryacquire(&q->lock) == 0)
      continue;
    STAILQ_FOREACH(p, &q->q, runqlink) {
      if (p->get_state() == RUNNABLE && !p->cpu_pin && 
          p->curcycles != 0 && p->curcycles > VICTIMAGE)
      {
        STAILQ_REMOVE(&q->q, p, proc, runqlink);
        steal = p;
        break;
      }
    }
    release(&q->lock);

    if (steal) {
      acquire(&steal->lock);
      if (steal->get_state() == RUNNABLE && !steal->cpu_pin &&
          steal->curcycles != 0 && steal->curcycles > VICTIMAGE)
      {
        steal->curcycles = 0;
        steal->cpuid = mycpu()->id;
        addrun(steal);
        release(&steal->lock);
        r = 1;
        break;
      }
      if (steal->get_state() == RUNNABLE)
        addrun(steal);
      release(&steal->lock);
    }
  }

  popcli();
  return r;
}

struct proc *
schednext(void)
{
  // No locks, interrupts enabled
  struct runq *q;
  struct proc *p = nullptr;

  pushcli();
  q = &runq[mycpu()->id];
  acquire(&q->lock);
  p = STAILQ_LAST(&q->q, proc, runqlink);
  if (p)
    STAILQ_REMOVE(&q->q, p, proc, runqlink);
  release(&q->lock);
  popcli();
  return p;
}

void
initsched(void)
{
  int i;

  for (i = 0; i < NCPU; i++) {
    initlock(&runq[i].lock, "runq", LOCKSTAT_SCHED);
    STAILQ_INIT(&runq[i].q);
  }
}

#if 0
static int
migrate(struct proc *p)
{
  // p should not be running, or be on a runqueue, or be myproc()
  int c;

  if (p == myproc())
    panic("migrate: myproc");

  for (c = 0; c < ncpu; c++) {
    if (c == mycpu()->id)
      continue;
    if (idle[c]) {    // OK if there is a race
      acquire(&p->lock);
      if (p->state == RUNNING)
        panic("migrate: pid %u name %s is running",
              p->pid, p->name);
      if (p->cpu_pin)
        panic("migrate: pid %u name %s is pinned",
              p->pid, p->name);

      p->curcycles = 0;
      p->cpuid = c;
      addrun(p);
      idle[c] = 0;

      release(&p->lock);
      return 0;
    }
  }

  return -1;
}
#endif
