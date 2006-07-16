#include "types.h"
#include "defs.h"
#include "x86.h"
#include "mmu.h"
#include "param.h"
#include "proc.h"
#include "spinlock.h"

// Can't call cprintf from inside these routines,
// because cprintf uses them itself.
#define cprintf dont_use_cprintf

extern int use_console_lock;

int
getcallerpc(void *v)
{
	return ((int*)v)[-1];
}

void
acquire1(struct spinlock * lock, struct proc *cp)
{
	if(cpus[cpu()].nlock++ == 0)
		cli();
	while(cmpxchg(0, 1, &lock->locked) == 1)
		;
	cpuid(0, 0, 0, 0, 0);	// memory barrier
	lock->locker_pc = getcallerpc(&lock);
}

void
release1(struct spinlock * lock, struct proc *cp)
{
	cpuid(0, 0, 0, 0, 0);	// memory barrier
	lock->locked = 0;
	if(--cpus[cpu()].nlock == 0)
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

