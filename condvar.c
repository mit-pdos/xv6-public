#include "types.h"
#include "amd64.h"
#include "mmu.h"
#include "spinlock.h"
#include "condvar.h"
#include "queue.h"
#include "proc.h"
#include "kernel.h"
#include "cpu.h"

void
initcondvar(struct condvar *cv, char *n)
{
  initlock(&cv->lock, n);
  cv->waiters = 0;
}

void
cv_sleep(struct condvar *cv, struct spinlock *lk)
{
  if(myproc() == 0)
    panic("sleep");

  if(lk == 0)
    panic("sleep without lk");

  // Must acquire cv_lock to avoid sleep/wakeup race
  acquire(&cv->lock); 

  release(lk);

  acquire(&myproc()->lock);

  if(myproc()->cv_next || myproc()->oncv)
    panic("cv_sleep cv_next");
  myproc()->cv_next = cv->waiters;
  cv->waiters = myproc();
  myproc()->state = SLEEPING;
  myproc()->oncv = cv;

  release(&cv->lock);

  sched();

  release(&myproc()->lock);

  // Reacquire original lock.
  acquire(lk);
}

// Wake up all processes sleeping on this condvar.
void
cv_wakeup(struct condvar *cv)
{
  // XXX race with cv_sleep()
  // if (!cv->waiters)
  //   return;

  acquire(&cv->lock);
  while(cv->waiters) {
    struct proc *p = cv->waiters;
    acquire(&p->lock);
    if(p->state != SLEEPING || p->oncv != cv)
      panic("cv_wakeup");
    struct proc *nxt = p->cv_next;
    p->cv_next = 0;
    p->oncv = 0;
    addrun(p);
    p->state = RUNNABLE;
    release(&p->lock);
    cv->waiters = nxt;
  }
  release(&cv->lock);
}
