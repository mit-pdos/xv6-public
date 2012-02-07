#include "types.h"
#include "amd64.h"
#include "mmu.h"
#include "spinlock.h"
#include "condvar.h"
#include "queue.h"
#include "proc.h"
#include "kernel.h"
#include "cpu.h"

struct spinlock tickslock __mpalign__;
struct condvar cv_ticks __mpalign__;
u64 ticks __mpalign__;

LIST_HEAD(sleepers, proc) sleepers __mpalign__;
struct spinlock sleepers_lock;

static void
wakeup(struct proc *p)
{
  LIST_REMOVE(p, cv_waiters);
  p->oncv = 0;
  addrun(p);
}

void
cv_tick(void)
{
  struct proc *p, *tmp;
  struct condvar *cv;
  int again;
  u64 now;
  
  acquire(&tickslock);
  ticks++;
  cv_wakeup(&cv_ticks);
  release(&tickslock);

  now = nsectime();
  again = 0;
  do {
    acquire(&sleepers_lock);
    LIST_FOREACH_SAFE(p, &sleepers, cv_sleep, tmp) {
      if (p->cv_wakeup <= now) {
        if (tryacquire(&p->lock)) {
          if (tryacquire(&p->oncv->lock)) {
            LIST_REMOVE(p, cv_sleep);
            cv = p->oncv;
            p->cv_wakeup = 0;
            wakeup(p);
            release(&p->lock);
            release(&cv->lock);
            continue;
          } else {
            release(&p->lock);
          }
        }
        again = 1;
      }
    }
    release(&sleepers_lock); 
  } while (again);
}

void cv_sleepto(struct condvar *cv, struct spinlock *lk, u64 timeout)
{
  if(myproc() == 0)
    panic("sleep");

  if(lk == 0)
    panic("sleep without lk");

  // Must acquire cv_lock to avoid sleep/wakeup race
  acquire(&cv->lock); 

  release(lk);

  acquire(&myproc()->lock);

  if(myproc()->oncv)
    panic("cv_sleep oncv");

  LIST_INSERT_HEAD(&cv->waiters, myproc(), cv_waiters);
  myproc()->oncv = cv;
  myproc()->state = SLEEPING;

  if (timeout) {
    acquire(&sleepers_lock);
    myproc()->cv_wakeup = timeout;
    LIST_INSERT_HEAD(&sleepers, myproc(), cv_sleep);
    release(&sleepers_lock); 
 }

  release(&cv->lock);

  sched();
  release(&myproc()->lock);

  // Reacquire original lock.
  acquire(lk);
}

void
cv_sleep(struct condvar *cv, struct spinlock *lk)
{
  cv_sleepto(cv, lk, 0);
}

// Wake up all processes sleeping on this condvar.
void
cv_wakeup(struct condvar *cv)
{
  struct proc *p, *tmp;

  acquire(&cv->lock);
  LIST_FOREACH_SAFE(p, &cv->waiters, cv_waiters, tmp) {
    acquire(&p->lock);
    if (p->state != SLEEPING)
      panic("cv_wakeup: pid %u name %s state %u",
            p->pid, p->name, p->state);
    if (p->oncv != cv)
      panic("cv_wakeup: pid %u name %s p->cv %p cv %p",
            p->pid, p->name, p->oncv, cv);
    if (p->cv_wakeup) {
      acquire(&sleepers_lock);
      LIST_REMOVE(p, cv_sleep);
      p->cv_wakeup = 0;
      release(&sleepers_lock);
    }
    wakeup(p);
    release(&p->lock);
  }
  release(&cv->lock);
}

void
initcondvar(struct condvar *cv, const char *n)
{
  initlock(&cv->lock, n, LOCKSTAT_CONDVAR);
  LIST_INIT(&cv->waiters);
}
