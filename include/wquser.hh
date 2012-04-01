#include "types.h"
#include "uspinlock.h"
#include "amd64.h"
#include "user.h"
#include "pthread.h"
#include "memlayout.h"
#include "uwq.hh"
#include "atomic.hh"
#include "lib.h"
#include "elf.hh"

typedef struct uspinlock wqlock_t;

int mycpuid(void);

static inline void*
allocwq(unsigned long nbytes)
{
  return malloc(nbytes);
}

static inline uwq_ipcbuf*
allocipc(void)
{
  static bool alloced;
  if (alloced)
    die("allocklen: allocing more than once");
  if (sizeof(uwq_ipcbuf) > USERWQSIZE)
    die("allocipc: too large");
  alloced = true;
  return (uwq_ipcbuf*)USERWQ;
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
#define pushcli()
#define popcli()

u64 wq_maxworkers = NWORKERS;
