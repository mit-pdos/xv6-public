#include "types.h"
#include "defs.h"
#include "x86.h"

#define LOCK_FREE -1

uint32_t kernel_lock = LOCK_FREE;

// lock = LOCK_FREE if free, else = cpu_id of owner CPU
void 
acquire_spinlock(uint32_t* lock)
{
  int cpu_id = lapic_cpu_number();
  cprintf ("acquire: %d\n", cpu_id);

  if (*lock == cpu_id)
    return;
  while ( cmpxchg(LOCK_FREE, cpu_id, lock) != cpu_id ) { ; }
}

void
release_spinlock(uint32_t* lock)
{
  int cpu_id = lapic_cpu_number();
  cprintf ("release: %d\n", cpu_id);
  if (*lock != cpu_id)
    panic("release_spinlock: releasing a lock that i don't own\n");
  *lock = LOCK_FREE;
}

void
release_grant_spinlock(uint32_t* lock, int c)
{
  int cpu_id = lapic_cpu_number();
  cprintf ("release_grant: %d -> %d\n", cpu_id, c);
  if (*lock != cpu_id)
    panic("release_spinlock: releasing a lock that i don't own\n");
  *lock = c;
}
