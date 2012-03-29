#include "types.h"
#include "stat.h"
#include "user.h"
#include "amd64.h"
#include "uspinlock.h"
#include "mtrace.h"
#include "pthread.h"

enum { readaccess = 1 };
enum { verbose = 0 };
enum { npg = 1 };

static pthread_barrier_t bar;
static pthread_barrier_t bar2;
#define NITER 1000000

void*
thr(void *arg)
{
  u64 tid = (u64)arg;
  if (setaffinity(tid) < 0)
    fprintf(1, "setaffinity err\n");

  pthread_barrier_wait(&bar);

  if (tid == 0)
    mtenable_type(mtrace_record_ascope, "xv6-asharing");

  pthread_barrier_wait(&bar2);

  for (int i = 0; i < NITER; i++) {
    if (verbose && ((i % 100) == 0))
      fprintf(1, "%d: %d ops\n", tid, i);

    volatile char *p = (char*) (0x100000000UL + tid * npg * 0x100000);
    if (map((void *) p, npg * 4096) < 0) {
      fprintf(1, "%d: map failed\n", tid);
      exit();
    }

    if (readaccess) {
      for (int j = 0; j < npg * 4096; j++)
	p[j] = '\0';
    }

    if (unmap((void *) p, npg * 4096) < 0) {
      fprintf(1, "%d: unmap failed\n", tid);
      exit();
    }
  }
  return 0;
}

int
main(int ac, char **av)
{
  if (ac != 2) {
    fprintf(1, "usage: %s nthreads\n", av[0]);
    exit();
  }

  int nthread = atoi(av[1]);

  // fprintf(1, "mapbench[%d]: start esp %x, nthread=%d\n", getpid(), rrsp(), nthread);
  pthread_barrier_init(&bar, 0, nthread);
  pthread_barrier_init(&bar2, 0, nthread);

  for(u64 i = 0; i < nthread; i++) {
    pthread_t tid;
    pthread_create(&tid, 0, thr, (void*) i);
    if (0) fprintf(1, "mapbench[%d]: child %d\n", getpid(), tid);
  }

  for(int i = 0; i < nthread; i++)
    wait();

  mtdisable("xv6-asharing");

  // fprintf(1, "mapbench[%d]: done\n", getpid());
  // halt();
  exit();
}
