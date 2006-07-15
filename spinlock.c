#include "types.h"
#include "defs.h"
#include "x86.h"
#include "mmu.h"
#include "param.h"
#include "proc.h"
#include "spinlock.h"

#define DEBUG 0

extern int use_console_lock;

int getcallerpc(void *v) {
  return ((int*)v)[-1];
}

void
acquire1(struct spinlock * lock, struct proc *cp)
{
  if(DEBUG) cprintf("cpu%d: acquiring at %x\n", cpu(), getcallerpc(&lock));

  cli();
  while ( cmpxchg(0, 1, &lock->locked) == 1 ) { ; }
  lock->locker_pc = getcallerpc(&lock);

  if(cp)
    cp->locks += 1;

  if(DEBUG) cprintf("cpu%d: acquired at %x\n", cpu(), getcallerpc(&lock));
}

void
release1(struct spinlock * lock, struct proc *cp)
{

  if(DEBUG) cprintf ("cpu%d: releasing at %x\n", cpu(), getcallerpc(&lock));

  if(lock->locked != 1)
    panic("release");

  if(cp)
    cp->locks -= 1;

  cmpxchg(1, 0, &lock->locked);
  sti();
}

void
acquire(struct spinlock *lock)
{
  acquire1(lock, curproc[cpu()]);
}

void
release(struct spinlock *lock)
{
  release1(lock, curproc[cpu()]);
}
