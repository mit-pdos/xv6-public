// Mutual exclusion spin locks.

#include "types.h"
#include "kernel.h"
#include "amd64.h"
#include "cpu.h"
#include "bits.h"
#include "spinlock.h"
#include "mtrace.h"

void
initlock(struct spinlock *lk, char *name)
{
#if SPINLOCK_DEBUG
  lk->name = name;
  lk->cpu = 0;
#endif
  lk->locked = 0;
}

// Acquire the lock.
// Loops (spins) until the lock is acquired.
// Holding a lock for a long time may cause
// other CPUs to waste time spinning to acquire it.
void
acquire(struct spinlock *lk)
{
  pushcli(); // disable interrupts to avoid deadlock.

#if SPINLOCK_DEBUG
  if(holding(lk)) {
    cprintf("%lx\n", __builtin_return_address(0));
    panic("acquire");
  }
#endif

  mtlock(lk);
  // The xchg is atomic.
  // It also serializes, so that reads after acquire are not
  // reordered before it.
  while(xchg32(&lk->locked, 1) != 0)
    ;
  mtacquired(lk);

#if SPINLOCK_DEBUG
  // Record info about lock acquisition for debugging.
  lk->cpu = mycpu();
  getcallerpcs(&lk, lk->pcs);
#endif
}

// Release the lock.
void
release(struct spinlock *lk)
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

// Record the current call stack in pcs[] by following the %ebp chain.
void
getcallerpcs(void *v, uptr pcs[])
{
  uptr *rbp;
  int i;

  rbp = (uptr*)v - 2;
  for(i = 0; i < 10; i++){
    if(rbp == 0 || rbp < (uptr*)KBASE || rbp == (uptr*)(~0UL))
      break;
    pcs[i] = rbp[1];     // saved %rip
    rbp = (uptr*)rbp[0]; // saved %rbp
  }
  for(; i < 10; i++)
    pcs[i] = 0;
}

// Check whether this cpu is holding the lock.
#if SPINLOCK_DEBUG
int
holding(struct spinlock *lock)
{
    return lock->locked && lock->cpu == mycpu();
}
#endif


// Pushcli/popcli are like cli/sti except that they are matched:
// it takes two popcli to undo two pushcli.  Also, if interrupts
// are off, then pushcli, popcli leaves them off.

void
pushcli(void)
{
  u64 rflags;

  rflags = readrflags();
  cli();
  if(mycpu()->ncli++ == 0)
    mycpu()->intena = rflags & FL_IF;
}

void
popcli(void)
{
  if(readrflags()&FL_IF)
    panic("popcli - interruptible");
  if(--mycpu()->ncli < 0)
    panic("popcli");
  if(mycpu()->ncli == 0 && mycpu()->intena)
    sti();
}
