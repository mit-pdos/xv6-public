#include "types.h"
#include "stat.h"
#include "user.h"
#include "xv6-mtrace.h"
#include "x86.h"
#include "uspinlock.h"

static struct uspinlock l;
static volatile uint tcount;
enum { nthread = 4 };

void
thr(uint tid)
{
  for (uint i = 0; i < 100; i++) {
    volatile char *p = (char*) (0x40000 + tid * 8 * 4096);
    if (map((void *) p, 8 * 4096) < 0) {
      printf(1, "%d: map failed\n", tid);
      exit();
    }

    for (uint j = 0; j < 8 * 4096; j++)
      p[j] = '\0';

    if (unmap((void *) p, 8 * 4096) < 0) {
      printf(1, "%d: unmap failed\n", tid);
      exit();
    }
  }

  acquire(&l);
  // printf(1, "mapbench[%d]: done\n", getpid());
  tcount++;
  release(&l);
  exit();
}

int
main(void)
{
  mtrace_enable_set(1, "xv6-mapbench");

  acquire(&l);
  printf(1, "mapbench[%d]: start esp %x\n", getpid(), resp());

  for(uint i = 0; i < nthread; i++) {
    sbrk(4096);
    uint *tstack = (uint*) sbrk(0);
    tstack[-1] = i;

    int tid = forkt(&tstack[-2], thr);
    if (0) printf(1, "mapbench[%d]: child %d\n", getpid(), tid);
  }

  for(;;){
    uint lastc = tcount;
    // printf(1, "mapbench[%d]: tcount=%d\n", getpid(), lastc);
    release(&l);
    if(lastc==nthread)
      break;
    while(tcount==lastc)
      __asm __volatile("");
    acquire(&l);
  }
  release(&l);
  printf(1, "mapbench[%d]: done\n", getpid());

  for(uint i = 0; i < nthread; i++)
    wait();

  mtrace_enable_set(0, "xv6-mapbench");
  halt();
  exit();
}
