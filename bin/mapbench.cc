#include "types.h"
#include "stat.h"
#include "user.h"
#include "mtrace.h"
#include "amd64.h"
#include "uspinlock.h"
#include "pthread.h"

enum { readaccess = 0 };
enum { verbose = 0 };
enum { npg = 1 };

void*
thr(void *arg)
{
  u64 tid = (u64)arg;

  for (int i = 0; i < 1000000; i++) {
    if (verbose && ((i % 100) == 0))
      fprintf(1, "%d: %d ops\n", tid, i);

    volatile char *p = (char*) (0x40000UL + tid * npg * 4096);
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
  mtenable("xv6-mapbench");

  if (ac != 2) {
    fprintf(1, "usage: %s nthreads\n", av[0]);
    exit();
  }

  int nthread = atoi(av[1]);

  // fprintf(1, "mapbench[%d]: start esp %x, nthread=%d\n", getpid(), rrsp(), nthread);

  for(u64 i = 0; i < nthread; i++) {
    pthread_t tid;
    pthread_create(&tid, 0, thr, (void*) i);
    if (0) fprintf(1, "mapbench[%d]: child %d\n", getpid(), tid);
  }

  for(int i = 0; i < nthread; i++)
    wait();

  // fprintf(1, "mapbench[%d]: done\n", getpid());
  mtdisable("xv6-mapbench");
  // halt();
  exit();
}
