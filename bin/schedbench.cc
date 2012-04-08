#include "types.h"
#include "user.h"
#include "amd64.h"
#include "pthread.h"
#include "futex.h"
#include "errno.h"
#include "atomic.hh"

static volatile std::atomic<u64> waiting;
static volatile std::atomic<u64> waking;
static volatile u64 ftx;
static int iters;
static int nworkers;

static
void* worker(void* x)
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
void master(void)
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

int
main(int ac, char** av)
{
  long r;

  if (ac < 3)
    die("usage: %s iters nworkers", av[0]);
  ftx = 0;
  iters = atoi(av[1]);
  nworkers = atoi(av[2]);
  waiting.store(0);

  for (int i = 0; i < nworkers; i++) {
    pthread_t th;

    r = pthread_create(&th, nullptr, worker, nullptr);
    if (r < 0)
      die("pthread_create");
  }
  nsleep(1000*1000);

  u64 t0 = rdtsc();
  master();
  u64 t1 = rdtsc();
  printf("%lu\n", (t1-t0)/iters);
  
  for (int i = 0; i < nworkers; i++)
    wait();
}
