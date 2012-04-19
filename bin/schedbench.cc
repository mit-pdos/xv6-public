#include "types.h"
#include "user.h"
#include "amd64.h"
#include "pthread.h"
#include "futex.h"
#include "errno.h"
#include "atomic.hh"
#include "mtrace.h"

static volatile std::atomic<u64> waiting;
static volatile std::atomic<u64> waking;
static int iters;
static int nworkers;
static volatile int go;

static struct {
  u64 mem;
  __padout__;
} ftx[256] __mpalign__;

static
void* worker0(void* x)
{
  // Ping pong a futex between a pair of workers
  u64 id = (u64)x;
  u64* f = &(ftx[id>>1].mem);
  long r;

  setaffinity(id);

  while (go == 0)
    yield();

  if (id & 0x1) {
    for (u64 i = 0; i < iters; i++) {
      r = futex(f, FUTEX_WAIT, (u64)(i<<1), 0);
      if (r < 0 && r != -EWOULDBLOCK)
        die("futex: %d", r);
      *f = (i<<1)+2;
      r = futex(f, FUTEX_WAKE, 1, 0);
      assert(r == 0);
    }
  } else {
    for (u64 i = 0; i < iters; i++) {
      *f = (i<<1)+1;
      r = futex(f, FUTEX_WAKE, 1, 0);
      assert(r == 0);
      r = futex(f, FUTEX_WAIT, (u64)(i<<1)+1, 0);
      if (r < 0 && r != -EWOULDBLOCK)
        die("futex: %d", r);
    }
  }

  return nullptr;
}

static
void master0(void)
{
  go = 1;
  for (int i = 0; i < nworkers; i++)
    wait();
}

int
main(int ac, char** av)
{
  long r;

  if (ac < 3)
    die("usage: %s iters nworkers", av[0]);

  iters = atoi(av[1]);
  nworkers = atoi(av[2]);
  waiting.store(0);

  for (int i = 0; i < nworkers; i++) {
    pthread_t th;

    r = pthread_create(&th, nullptr, worker0, (void*)(u64)i);
    if (r < 0)
      die("pthread_create");
  }
  nsleep(1000*1000);

  mtenable("xv6-schedbench");
  u64 t0 = rdtsc();
  master0();
  u64 t1 = rdtsc();
  mtdisable("xv6-schedbench");
  printf("%lu\n", (t1-t0)/iters);
}

#if 0
static volatile u64 ftx;

static
void* worker0(void* x)
{
  long r;
  int i;

  for (i = 0; i < iters; i++) {
    ++waiting;
    r = futex((u64*)&ftx, FUTEX_WAIT, (u64)i, 0);
    if (r < 0 && r != -EWOULDBLOCK)
      die("FUTEX_WAIT: %d", r);
    while (waking.load() == 1)
      nop_pause();
  }

  return nullptr;
}

static
void master0(void)
{
  long r;

  for (int i = 0; i < iters; i++) {
    while (waiting.load() < (i+1)*nworkers)
      nop_pause();
    
    waking.store(1);
    ftx = i+1;
    r = futex((u64*)&ftx, FUTEX_WAKE, nworkers, 0);  
    assert(r == 0);
    waking.store(0);
  }
}
#endif
