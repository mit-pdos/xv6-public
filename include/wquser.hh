#include "types.h"
#include "uspinlock.h"
#include "amd64.h"
#include "user.h"
#include "wq.hh"
#include "pthread.h"

typedef struct uspinlock wqlock_t;

static pthread_key_t idkey;
static volatile int exiting;

int
mycpuid(void)
{
  return (int)(u64)pthread_getspecific(idkey);
}

static inline void*
allocwq(unsigned long nbytes)
{
  return malloc(nbytes);
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

static void
setaffinity(int c)
{
  // XXX(sbw)
}

static void*
workerth(void *x)
{
  u64 c = (u64)x;
  
  setaffinity(c);
  pthread_setspecific(idkey, (void*)c);
  while (!exiting)
    wq_trywork();

  return 0;
}

static inline void
wqarch_init(void)
{
  pthread_t th;
  int r;

  if (pthread_key_create(&idkey, 0))
    die("wqarch_init: pthread_key_create");

  pthread_setspecific(idkey, 0);
  setaffinity(0);

  for (int i = 1; i < NCPU; i++) {
    r = pthread_create(&th, 0, workerth, (void*)(u64)i);
    if (r < 0)
      die("wqarch_init: pthread_create");
  }
}

static inline void
wqarch_exit(void)
{
  exiting = 1;
}

#define xprintf      printf 
#define pushcli()
#define popcli()
