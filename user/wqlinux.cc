#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "types.h"
#include "wq.hh"
#include "user/util.h"

static __thread int myid_;
static wq *wq_;
u64 wq_maxworkers = NCPU-1;

int
mycpuid(void)
{
  return myid_;
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

void*
xallocwork(unsigned long nbytes)
{
  return malloc(nbytes);
}

void 
xfreework(void* ptr, unsigned long nbytes)
{
  free(ptr);
}

void
initwq(void)
{
  wq_ = new wq();
  if (wq_ == nullptr)
    die("initwq");

  pthread_t th;
  int r;

  myid_ = 0;
  setaffinity(0);

  for (int i = 1; i < wq_maxworkers+1; i++) {
    r = pthread_create(&th, NULL, workerth, (void*)(u64)i);
    if (r < 0)
      edie("pthread_create");
  }
}
