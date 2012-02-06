// Mutual exclusion spin locks.

#include "types.h"
#include "kernel.h"
#include "amd64.h"
#include "cpu.h"
#include "bits.h"
#include "spinlock.h"
#include "mtrace.h"

static inline void
locking(struct spinlock *lk)
{
#if SPINLOCK_DEBUG
  if(holding(lk)) {
    cprintf("%p\n", __builtin_return_address(0));
    panic("acquire");
  }
#endif

  mtlock(lk);
}

static inline void
locked(struct spinlock *lk)
{
  mtacquired(lk);
  
#if SPINLOCK_DEBUG
  // Record info about lock acquisition for debugging.
  lk->cpu = mycpu();
  getcallerpcs(&lk, lk->pcs, NELEM(lk->pcs));
#endif
}

static inline void
releasing(struct spinlock *lk)
{
#if SPINLOCK_DEBUG
  if(!holding(lk)) {
    cprintf("lock: %s\n", lk->name);
    panic("release");
  }
#endif

  mtunlock(lk);

#if SPINLOCK_DEBUG
  lk->pcs[0] = 0;
  lk->cpu = 0;
#endif
}

// Check whether this cpu is holding the lock.
#if SPINLOCK_DEBUG
int
holding(struct spinlock *lock)
{
  return lock->locked && lock->cpu == mycpu();
}
#endif

void
initlock(struct spinlock *lk, const char *name)
{
#if SPINLOCK_DEBUG
  lk->name = name;
  lk->cpu = 0;
#endif
  lk->locked = 0;
}

int
tryacquire(struct spinlock *lk)
{
  pushcli();
  locking(lk);
  if (xchg32(&lk->locked, 1) != 0) {
      popcli();
      return 0;
  }
  locked(lk);
  return 1;
}

// Acquire the lock.
// Loops (spins) until the lock is acquired.
// Holding a lock for a long time may cause
// other CPUs to waste time spinning to acquire it.
void
acquire(struct spinlock *lk)
{
  pushcli();
  locking(lk);
  // The xchg is atomic.
  // It also serializes, so that reads after acquire are not
  // reordered before it.
  while(xchg32(&lk->locked, 1) != 0)
    ;
  locked(lk);
}

// Release the lock.
void
release(struct spinlock *lk)
{
  releasing(lk);

  // The xchg serializes, so that reads before release are
  // not reordered after it.  The 1996 PentiumPro manual (Volume 3,
  // 7.2) says reads can be carried out speculatively and in
  // any order, which implies we need to serialize here.
  // But the 2007 Intel 64 Architecture Memory Ordering White
  // Paper says that Intel 64 and IA-32 will not move a load
  // after a store. So lock->locked = 0 would work here.
  // The xchg being asm volatile ensures gcc emits it after
  // the above assignments (and after the critical section).
  xchg32(&lk->locked, 0);

  popcli();
}
