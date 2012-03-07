#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <pthread.h>

#include "user/util.h"
#include "include/types.h"
#include "include/wq.hh"

static __thread int myid_;

typedef pthread_spinlock_t wqlock_t;

int
mycpuid(void)
{
  return myid_;
}

static inline void*
allocwq(void)
{
  return malloc(WQSIZE);
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

static void*
workerth(void *x)
{
  u64 c = (u64)x;
  myid_ = c;
  setaffinity(c);
  while (1)
    wq_trywork();

  return NULL;
}

static inline void
wqarch_init(void)
{
  pthread_t th;
  int r;

  myid_ = 0;
  setaffinity(0);

  for (int i = 1; i < NCPU; i++) {
    r = pthread_create(&th, NULL, workerth, (void*)(u64)i);
    if (r < 0)
      edie("pthread_create");
  }
}

#define xprintf        printf
#define pushcli()
#define popcli()
