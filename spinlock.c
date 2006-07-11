#include "types.h"
#include "defs.h"
#include "x86.h"
#include "mmu.h"

#define LOCK_FREE -1
#define DEBUG 0

uint32_t kernel_lock = LOCK_FREE;

int getcallerpc(void *v) {
  return ((int*)v)[-1];
}

// lock = LOCK_FREE if free, else = cpu_id of owner CPU
void
acquire_spinlock(uint32_t* lock)
{
  int cpu_id = cpu();

  // on a real machine there would be a memory barrier here
  if(DEBUG) cprintf("cpu%d: acquiring at %x\n", cpu_id, getcallerpc(&lock));
  cli();
  if (*lock == cpu_id)
    panic("recursive lock");
  
  while ( cmpxchg(LOCK_FREE, cpu_id, lock) != cpu_id ) { ; }
  if(DEBUG) cprintf("cpu%d: acquired at %x\n", cpu_id, getcallerpc(&lock));
}

void
release_spinlock(uint32_t* lock)
{
  int cpu_id = cpu();
  if(DEBUG) cprintf ("cpu%d: releasing at %x\n", cpu_id, getcallerpc(&lock));
  if (*lock != cpu_id)
    panic("release_spinlock: releasing a lock that i don't own\n");
  *lock = LOCK_FREE;
  // on a real machine there would be a memory barrier here
  sti();
}

void
release_grant_spinlock(uint32_t* lock, int c)
{
  int cpu_id = cpu();
  if(DEBUG) cprintf ("cpu%d: release_grant to %d at %x\n", cpu_id, c, getcallerpc(&lock));
  if (*lock != cpu_id)
    panic("release_spinlock: releasing a lock that i don't own\n");
  *lock = c;
}

