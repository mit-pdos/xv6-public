#include "types.h"
#include "uspinlock.h"
#include "amd64.h"
#include "user.h"
#include "wq.hh"
#include "pthread.h"
#include "memlayout.h"
#include "uwq.hh"
#include "atomic.hh"
#include "lib.h"
#include "elf.hh"

typedef struct uspinlock wqlock_t;

static pthread_key_t idkey;
static std::atomic<int> nextid;
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

extern "C" long wqwait(void);

static void __attribute__((used))
initworker(void)
{
  int id;
  forkt_setup(0);
  id = nextid++;
  if (id >= NCPU)
    die("initworker: to man IDs");
  pthread_setspecific(idkey, (void*)(u64)id);
  while (1) {
    if (!wq_trywork())
      assert(wqwait() == 0);
  }
}
DEFINE_XV6_ADDRNOTE(xnote, XV6_ADDR_ID_WQ, &initworker);

static inline void
wqarch_init(void)
{
  if (pthread_key_create(&idkey, 0))
    die("wqarch_init: pthread_key_create");

  int id = nextid++;
  pthread_setspecific(idkey, (void*)(u64)id);
}

#define xprintf      printf 
#define pushcli()
#define popcli()

u64 wq_maxworkers = NWORKERS;
