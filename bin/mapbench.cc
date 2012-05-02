#if defined(LINUX)
#include <pthread.h>
#include "user/util.h"
#include "types.h"
#include <assert.h>
#include <sys/wait.h>
#include <unistd.h>
#else
#include "types.h"
#include "stat.h"
#include "user.h"
#include "amd64.h"
#include "uspinlock.h"
#include "mtrace.h"
#include "pthread.h"
#endif
#include "xsys.h"
#include <sys/mman.h>

enum { readaccess = 1 };
enum { verbose = 0 };
enum { npg = 1 };

static pthread_barrier_t bar;
static pthread_barrier_t bar2;
static int niter;

void*
thr(void *arg)
{
  u64 tid = (u64)arg;

  if (setaffinity(tid) < 0)
    die("setaffinity err");

  pthread_barrier_wait(&bar);

  if (tid == 0)
    mtenable_type(mtrace_record_ascope, "xv6-asharing");

  pthread_barrier_wait(&bar2);

  for (int i = 0; i < niter; i++) {
    if (verbose && ((i % 100) == 0))
      printf("%d: %d ops\n", tid, i);

    volatile char *p = (char*) (0x100000000UL + tid * npg * 0x100000);
    if (mmap((void *) p, npg * 4096, PROT_READ|PROT_WRITE,
             MAP_PRIVATE|MAP_FIXED|MAP_ANONYMOUS, -1, 0) == MAP_FAILED) {
      die("%d: map failed", tid);
    }

    if (readaccess) {
      for (int j = 0; j < npg * 4096; j++)
	p[j] = '\0';
    }

    if (munmap((void *) p, npg * 4096) < 0) {
      die("%d: unmap failed\n", tid);
    }
  }
  return 0;
}

int
main(int ac, char **av)
{
  if (ac < 2)
    die("usage: %s nthreads [nloop]", av[0]);

  int nthread = atoi(av[1]);
  niter = 100;
  if (ac > 2)
    niter = atoi(av[2]);

  pthread_barrier_init(&bar, 0, nthread);
  pthread_barrier_init(&bar2, 0, nthread);

  for(u64 i = 0; i < nthread; i++) {
    pthread_t tid;
    pthread_create(&tid, 0, thr, (void*) i);
    if (0) printf("mapbench[%d]: child %d\n", getpid(), tid);
  }

  for(int i = 0; i < nthread; i++)
    xwait();

  mtdisable("xv6-asharing");

  // fprintf(1, "mapbench[%d]: done\n", getpid());
  // halt();
  xexit();
}
