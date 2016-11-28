// Mutual exclusion spin locks.

#include <stddef.h>
#include <stdatomic.h>

#include "types.h"
#include "defs.h"
#include "param.h"
#include "x86.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "spinlock.h"

void
initlock(struct spinlock *lk, char *name)
{
  lk->name = name;
  lk->locked = ATOMIC_VAR_INIT(0);
  lk->cpu = ATOMIC_VAR_INIT(NULL);
}

// Acquire the lock.
// Loops (spins) until the lock is acquired.
// Holding a lock for a long time may cause
// other CPUs to waste time spinning to acquire it.
void
acquire(struct spinlock *lk)
{
  pushcli(); // disable interrupts to avoid deadlock.
  if(holding(lk))
    panic("acquire");

  // The atomic_exchange_explicit is atomic.
  while (atomic_exchange_explicit(&lk->locked, 1, memory_order_relaxed))
    ;

  // Tell the C compiler and the processor to not move loads or stores
  // past this point, to ensure that the critical section's memory
  // references happen after the lock is acquired.
  atomic_thread_fence(memory_order_acquire);

  // Record info about lock acquisition for debugging.
  atomic_store_explicit(&lk->cpu, get_cpu(), memory_order_relaxed);
  getcallerpcs(&lk, lk->pcs);
}

// Release the lock.
void
release(struct spinlock *lk)
{
  if(!holding(lk))
    panic("release");

  lk->pcs[0] = 0;
  atomic_store_explicit(&lk->cpu, NULL, memory_order_relaxed);

  // Tell the C compiler and the processor to not move loads or stores
  // past this point, to ensure that all the stores in the critical
  // section are visible to other cores before the lock is released.
  // Both the C compiler and the hardware may re-order loads and
  // stores; memory_order_release tells them both not to.
  //
  // Release the lock, equivalent to lk->locked = 0.
  atomic_store_explicit(&lk->locked, 0, memory_order_release);

  popcli();
}

// Record the current call stack in pcs[] by following the %ebp chain.
void
getcallerpcs(void *v, uint pcs[])
{
  uint *ebp;
  int i;

  ebp = (uint*)v - 2;
  for(i = 0; i < 10; i++){
    if(ebp == 0 || ebp < (uint*)KERNBASE || ebp == (uint*)0xffffffff)
      break;
    pcs[i] = ebp[1];     // saved %eip
    ebp = (uint*)ebp[0]; // saved %ebp
  }
  for(; i < 10; i++)
    pcs[i] = 0;
}

// Check whether this cpu is holding the lock.
int
holding(struct spinlock *lock)
{
  return
    atomic_load_explicit(&lock->locked, memory_order_relaxed) &&
    atomic_load_explicit(&lock->cpu, memory_order_relaxed) == get_cpu();
}


// Pushcli/popcli are like cli/sti except that they are matched:
// it takes two popcli to undo two pushcli.  Also, if interrupts
// are off, then pushcli, popcli leaves them off.

void
pushcli(void)
{
  int eflags;

  eflags = readeflags();
  cli();
  if(get_cpu()->ncli == 0)
    get_cpu()->intena = eflags & FL_IF;
  get_cpu()->ncli += 1;
}

void
popcli(void)
{
  if(readeflags()&FL_IF)
    panic("popcli - interruptible");
  if(--get_cpu()->ncli < 0)
    panic("popcli");
  if(get_cpu()->ncli == 0 && get_cpu()->intena)
    sti();
}

