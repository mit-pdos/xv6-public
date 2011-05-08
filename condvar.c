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

  cprintf("cv_sleep: 0x%x %s\n", cv, cv->lock.name);

  release(lk);

  if (cv->waiters != 0)
    panic("cv_sleep\n");

  cv->waiters = proc;  // XXX should be queue

  delrun(proc);

  acquire(&proc->lock);

  release(&cv->lock);

  proc->state = SLEEPING;

  sched();

  release(&proc->lock);

  acquire(&cv->lock);

  // Tidy up.
  cv->waiters = 0;

  // Reacquire original lock.
  cprintf("acquire %s\n", lk->name);
  acquire(lk);

  release(&cv->lock);
}

void
cv_wakeup(struct condvar *cv)
{
  acquire(&cv->lock);
  if (cv->waiters != 0) {
    cprintf("wakeup 0x%x %d\n", cv, cv->waiters->pid);
    addrun(cv->waiters);
  }
  release(&cv->lock);
}
