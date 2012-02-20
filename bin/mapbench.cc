#include "types.h"
#include "stat.h"
#include "user.h"
#include "mtrace.h"
#include "amd64.h"
#include "uspinlock.h"

static struct uspinlock l;
static volatile int tcount;
enum { readaccess = 1 };
enum { verbose = 0 };

void
thr(void *arg)
{
  u64 tid = (u64)arg;

  for (int i = 0; i < 10000; i++) {
    if (verbose && ((i % 100) == 0))
      fprintf(1, "%d: %d ops\n", tid, i);

    volatile char *p = (char*) (0x40000UL + tid * 8 * 4096);
    if (map((void *) p, 8 * 4096) < 0) {
      fprintf(1, "%d: map failed\n", tid);
      exit();
    }

    if (readaccess) {
      for (int j = 0; j < 8 * 4096; j++)
	p[j] = '\0';
    }

    if (unmap((void *) p, 8 * 4096) < 0) {
      fprintf(1, "%d: unmap failed\n", tid);
      exit();
    }
  }

  acquire(&l);
  // fprintf(1, "mapbench[%d]: done\n", getpid());
  tcount++;
  release(&l);
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

  acquire(&l);
  // fprintf(1, "mapbench[%d]: start esp %x, nthread=%d\n", getpid(), rrsp(), nthread);

  for(int i = 0; i < nthread; i++) {
    sbrk(8192);
    void *tstack = sbrk(0);
    // fprintf(1, "tstack %lx\n", tstack);
    int tid = forkt(tstack, (void*) thr, (void *)(u64)i);
    if (0) fprintf(1, "mapbench[%d]: child %d\n", getpid(), tid);
  }

  for(;;){
    int lastc = tcount;
    // fprintf(1, "mapbench[%d]: tcount=%d\n", getpid(), lastc);
    release(&l);
    if(lastc==nthread)
      break;
    while(tcount==lastc)
      __asm __volatile("":::"memory");
    acquire(&l);
  }
  release(&l);
  // fprintf(1, "mapbench[%d]: done\n", getpid());

  for(int i = 0; i < nthread; i++)
    wait();

  mtdisable("xv6-mapbench");
  // halt();
  exit();
}
