#include "types.h"
#include "defs.h"
#include "x86.h"
#include "mmu.h"
#include "param.h"
#include "proc.h"
#include "spinlock.h"

#define DEBUG 0

int getcallerpc(void *v) {
  return ((int*)v)[-1];
}

void
acquire(struct spinlock * lock)
{
  struct proc * cp = curproc[cpu()];

  // on a real machine there would be a memory barrier here
  if(DEBUG) cprintf("cpu%d: acquiring at %x\n", cpu(), getcallerpc(&lock));
  if (cp && lock->p == cp && lock->locked){
    lock->count += 1;
  } else { 
    cli();
    while ( cmpxchg(0, 1, &lock->locked) != 1 ) { ; }
    lock->locker_pc = getcallerpc(&lock);
    lock->count = 1;
    lock->p = cp;
  }
  if(DEBUG) cprintf("cpu%d: acquired at %x\n", cpu(), getcallerpc(&lock));
}

void
release(struct spinlock * lock)
{
  struct proc * cp = curproc[cpu()];

  if(DEBUG) cprintf ("cpu%d: releasing at %x\n", cpu(), getcallerpc(&lock));

  if(lock->p != cp || lock->count < 1 || lock->locked != 1)
    panic("release");

  lock->count -= 1;
  if(lock->count < 1){
    lock->p = 0;
    cmpxchg(1, 0, &lock->locked);
    sti();
    // on a real machine there would be a memory barrier here
  }
}
