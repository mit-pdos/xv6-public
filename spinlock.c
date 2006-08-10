#include "types.h"
#include "defs.h"
#include "x86.h"
#include "mmu.h"
#include "param.h"
#include "proc.h"
#include "spinlock.h"

// Can't call cprintf from inside these routines,
// because cprintf uses them itself.
//#define cprintf dont_use_cprintf

#define LOCKMAGIC 0x6673ffea

extern int use_console_lock;

void
initlock(struct spinlock *lock, char *name)
{
  lock->magic = LOCKMAGIC;
  lock->name = name;
  lock->locked = 0;
  lock->cpu = 0xffffffff;
}

void
getcallerpcs(void *v, uint pcs[])
{
  uint *ebp = (uint*)v - 2;
  int i;
  for(i = 0; i < 10 && ebp && ebp != (uint*)0xffffffff; ebp = (uint*)*ebp, i++){
    pcs[i] = *(ebp + 1);
  }
  for( ; i < 10; i++)
    pcs[i] = 0;
}

void
acquire(struct spinlock * lock)
{
  if(lock->magic != LOCKMAGIC)
    panic("weird lock magic");
  if(holding(lock))
    panic("acquire");

  if(cpus[cpu()].nlock == 0)
    cli();
  cpus[cpu()].nlock++;
                
	while(cmpxchg(0, 1, &lock->locked) == 1)
		;
	cpuid(0, 0, 0, 0, 0);	// memory barrier
	getcallerpcs(&lock, lock->pcs);
	lock->cpu = cpu() + 10;
        cpus[cpu()].lastacquire = lock;
}

void
release(struct spinlock * lock)
{
  if(lock->magic != LOCKMAGIC)
    panic("weird lock magic");

	if(!holding(lock))
		panic("release");

        cpus[cpu()].lastrelease = lock;
        lock->pcs[0] = 0;
        lock->cpu = 0xffffffff;
	cpuid(0, 0, 0, 0, 0);	// memory barrier
	lock->locked = 0;
	if(--cpus[cpu()].nlock == 0)
		sti();
}

int
holding(struct spinlock *lock)
{
	return lock->locked && lock->cpu == cpu() + 10;
}
