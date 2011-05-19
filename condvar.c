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

  if(proc->cv_next)
    panic("cv_sleep cv_next");

  proc->cv_next = cv->waiters;
  cv->waiters = proc;

  acquire(&proc->lock);

  release(&cv->lock);

  proc->state = SLEEPING;

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
    if(cv->waiters->state != SLEEPING)
      panic("cv_wakeup");
    struct proc *nxt = cv->waiters->cv_next;
    cv->waiters->cv_next = 0;
    addrun(cv->waiters);
    cv->waiters = nxt;
  }
  release(&cv->lock);
}
