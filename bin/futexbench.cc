#include "types.h"
#include "user.h"
#include "amd64.h"
#include "pthread.h"
#include "futex.h"
#include "errno.h"

static volatile u64 ftx;
static int iters;

static void*
worker(void *arg)
{
  long r;

  for (int i = 0; i < iters; i++) {
    r = futex((u64*)&ftx, FUTEX_WAIT, 0, 0);
    if (r < 0 && r != -EWOULDBLOCK)
      die("FUTEX_WAIT: %d", r);
    ftx = 0;
    r = futex((u64*)&ftx, FUTEX_WAKE, 1, 0);    
  }
  return nullptr;
}

int
main(int ac, char** av)
{
  pthread_t th;
  long r;

  if (ac < 2)
    die("usage: %s iters", av[0]);

  iters = atoi(av[1]);

  pthread_create(&th, 0, &worker, 0);
  for (int i = 0; i < iters; i++) {
    ftx = 1;
    r = futex((u64*)&ftx, FUTEX_WAKE, 1, 0);
    assert(r == 0);
    r = futex((u64*)&ftx, FUTEX_WAIT, 1, 0);    
    if (r < 0 && r != -EWOULDBLOCK)
      die("FUTEX_WAIT: %d", r);
  }

  wait();
  printf("done\n");
  return 0;
}
