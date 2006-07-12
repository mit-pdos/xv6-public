#include "types.h"
#include "defs.h"
#include "x86.h"
#include "mmu.h"
#include "param.h"
#include "proc.h"
#include "spinlock.h"

#define DEBUG 0

extern use_printf_lock;

int getcallerpc(void *v) {
  return ((int*)v)[-1];
}

void
acquire(struct spinlock * lock)
{
  struct proc *cp = curproc[cpu()];
  unsigned who;

  if(cp)
    who = (unsigned) cp;
  else
    who = cpu() + 1;

  if(DEBUG) cprintf("cpu%d: acquiring at %x\n", cpu(), getcallerpc(&lock));

  if (lock->who == who && lock->locked){
    lock->count += 1;
  } else { 
    cli();
    // if we get the lock, eax will be zero
    // if we don't get the lock, eax will be one
    while ( cmpxchg(0, 1, &lock->locked) == 1 ) { ; }
    lock->locker_pc = getcallerpc(&lock);
    lock->count = 1;
    lock->who = who;
  }

  if(cp)
    cp->locks += 1;

  if(DEBUG) cprintf("cpu%d: acquired at %x\n", cpu(), getcallerpc(&lock));
}

void
release(struct spinlock * lock)
{
  struct proc *cp = curproc[cpu()];
  unsigned who;

  if(cp)
    who = (unsigned) cp;
  else
    who = cpu() + 1;

  if(DEBUG) cprintf ("cpu%d: releasing at %x\n", cpu(), getcallerpc(&lock));

  if(lock->who != who || lock->count < 1 || lock->locked != 1)
    panic("release");

  lock->count -= 1;
  if(cp)
    cp->locks -= 1;
  if(lock->count < 1){
    lock->who = 0;
    cmpxchg(1, 0, &lock->locked);
    sti();
  }
}
