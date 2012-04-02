#include "wqtypes.hh"
#include "kernel.hh"
#include "spinlock.h"
#include "amd64.h"
#include "cpu.hh"
#include "kalloc.hh"

static inline void*
allocwq(unsigned long nbytes)
{
  return ksalloc(slab_wq);
}

static inline void
freewq(void* p)
{
  ksfree(slab_wq, p);
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

#define xprintf      cprintf 
