#include "types.h"
#include "defs.h"
#include "param.h"
#include "x86.h"
#include "mmu.h"
#include "spinlock.h"
#include "condvar.h"
#include "queue.h"
#include "proc.h"

void
initcondvar(struct condvar *cv, char *n)
{
  initlock(&cv->lock, n);
  cv->waiters = 0;
}

void
cv_sleep(struct condvar *cv, struct spinlock *lk)
{
  if(proc == 0)
    panic("sleep");

  if(lk == 0)
    panic("sleep without lk");

  // Must acquire cv_lock to avoid sleep/wakeup race
  acquire(&cv->lock); 

  release(lk);

  acquire(&proc->lock);

  if(proc->cv_next || proc->oncv)
    panic("cv_sleep cv_next");
  proc->cv_next = cv->waiters;
  cv->waiters = proc;
  proc->state = SLEEPING;
  proc->oncv = cv;

  release(&cv->lock);

  sched();

  release(&proc->lock);

  // Reacquire original lock.
  acquire(lk);
}

// Wake up all processes sleeping on this condvar.
void
cv_wakeup(struct condvar *cv)
{
  acquire(&cv->lock);
  while(cv->waiters) {
    struct proc *p = cv->waiters;
    if(p->state != SLEEPING || p->oncv != cv)
      panic("cv_wakeup");
    struct proc *nxt = p->cv_next;
    p->cv_next = 0;
    p->oncv = 0;
    addrun(p);
    cv->waiters = nxt;
  }
  release(&cv->lock);
}
