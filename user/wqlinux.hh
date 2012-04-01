#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

#include "types.h"
#include "wqtypes.hh"

int mycpuid(void);

static inline void*
allocwq(unsigned long nbytes)
{
  return malloc(nbytes);
}

static inline void
wqlock_acquire(wqlock_t *lock)
{
  pthread_spin_lock(lock);
}

static inline int
wqlock_tryacquire(wqlock_t *lock)
{
  return (pthread_spin_trylock(lock) == 0);
}

static inline void
wqlock_release(wqlock_t *lock)
{
  pthread_spin_unlock(lock);
}

static inline void
wqlock_init(wqlock_t *lock) 
{
  pthread_spin_init(lock, 0);
}

static inline u64
rdtsc(void)
{
  u32 hi, lo;
  __asm volatile("rdtsc" : "=a"(lo), "=d"(hi));
  return ((u64)lo)|(((u64)hi)<<32);
}

#define xprintf        printf
