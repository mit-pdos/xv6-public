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

extern int use_console_lock;

int
getcallerpc(void *v)
{
	return ((int*)v)[-1];
}

void
acquire(struct spinlock * lock)
{
  if(holding(lock)){
    extern use_console_lock;
    use_console_lock = 0;
    cprintf("lock %s pc %x\n", lock->name ? lock->name : "", lock->pc);
		panic("acquire");
  }

	if(cpus[cpu()].nlock++ == 0)
		cli();
	while(cmpxchg(0, 1, &lock->locked) == 1)
		;
	cpuid(0, 0, 0, 0, 0);	// memory barrier
	lock->pc = getcallerpc(&lock);
	lock->cpu = cpu();
        cpus[cpu()].lastacquire = lock;
}

void
release(struct spinlock * lock)
{
	if(!holding(lock))
		panic("release");

        cpus[cpu()].lastrelease = lock;
	cpuid(0, 0, 0, 0, 0);	// memory barrier
	lock->locked = 0;
	if(--cpus[cpu()].nlock == 0)
		sti();
}

int
holding(struct spinlock *lock)
{
	return lock->locked && lock->cpu == cpu();
}
