#include "types.h"
#include "user.h"
#include "uwq.hh"
#include "wqtypes.hh"
#include "wq.hh"
#include "atomic.hh"
#include "pthread.h"
#include "memlayout.h"

u64 wq_maxworkers = NWORKERS;

static pthread_key_t idkey;
static std::atomic<int> nextid;

static wq *wq_;

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

uwq_ipcbuf*
allocipc(void)
{
  static bool alloced;
  if (alloced)
    die("allocklen: allocing more than once");
  if (sizeof(uwq_ipcbuf) > USERWQSIZE)
    die("allocipc: too large");
  if (wqinit((uptr)initworker) < 0)
    die("wqinit: failed");
  alloced = true;
  return (uwq_ipcbuf*)USERWQ;
}

int
mycpuid(void)
{
  return (int)(u64)pthread_getspecific(idkey);
}

size_t
wq_size(void)
{
  return sizeof(wq);
}

int
wq_push(work *w)
{
  return wq_->push(w, mycpuid());
}

int
wq_pushto(work *w, int tcpuid)
{
  return wq_->push(w, tcpuid);
}

int
wq_trywork(void)
{
  return wq_->trywork();
}

void
wq_dump(void)
{
  return wq_->dump();
}

void*
xallocwork(unsigned long nbytes)
{
  return wqalloc(nbytes);
}

void 
xfreework(void* ptr, unsigned long nbytes)
{
  wqfree(ptr);
}

void
initwq(void)
{
  extern void wqallocinit(void);

  if (pthread_key_create(&idkey, 0))
    die("wqarch_init: pthread_key_create");

  int id = nextid++;
  pthread_setspecific(idkey, (void*)(u64)id);

  wqallocinit();

  wq_ = new wq();
  if (wq_ == nullptr)
    die("initwq");
}
