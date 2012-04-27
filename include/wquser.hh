#include "types.h"
#include "uspinlock.h"
#include "amd64.h"
#include "user.h"
#include "uwq.hh"
#include "wqtypes.hh"

uwq_ipcbuf* allocipc(void);

static inline void*
allocwq(unsigned long nbytes)
{
  return malloc(nbytes);
}

static inline void
freewq(void* p)
{
  free(p);
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
  initlock(lock);
}

#define xprintf      printf 
