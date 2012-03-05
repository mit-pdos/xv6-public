#include "types.h"
#include "kernel.hh"
#include "spinlock.h"
#include "amd64.h"
#include "cpu.hh"
#include "kalloc.hh"
#include "wq.hh"

typedef struct spinlock wqlock_t;

static inline void*
allocwq(void)
{
  return ksalloc(slab_wq);
}

static inline void
wqlock_acquire(wqlock_t *lock)
{
  acquire(lock);
}

static inline int
wqlock_tryacquire(wqlock_t *lock)
{
  return tryacquire(lock);
}

static inline void
wqlock_release(wqlock_t *lock)
{
  release(lock);
}

static inline void
wqlock_init(wqlock_t *lock) 
{
  initlock(lock, "wq lock", LOCKSTAT_WQ);
}

static inline void
wqarch_init(void)
{
}

#define xprintf      cprintf 
