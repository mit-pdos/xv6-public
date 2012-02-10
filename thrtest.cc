extern "C" {
#include "types.h"
#include "stat.h"
#include "user.h"
#include "mtrace.h"
#include "amd64.h"
#include "uspinlock.h"
}

static struct uspinlock l;
static volatile int tcount;
enum { nthread = 8 };

void
thr(void *arg)
{
  acquire(&l);
  printf(1, "thrtest[%d]: arg 0x%lx rsp %lx\n", getpid(), arg, rrsp());
  tcount++;
  release(&l);
  exit();
}

int
main(void)
{
  acquire(&l);
  printf(1, "thrtest[%d]: start esp %x\n", getpid(), rrsp());

  for(int i = 0; i < nthread; i++) {
    sbrk(8192);
    void *tstack = sbrk(0);
    int tid = forkt(tstack, (void*) thr, (void*)(u64)(0xc0ffee00|i));
    printf(1, "thrtest[%d]: child %d\n", getpid(), tid);
  }

  for(;;){
    int lastc = tcount;
    printf(1, "thrtest[%d]: tcount=%d\n", getpid(), lastc);
    release(&l);
    if(lastc==nthread)
      break;
    while(tcount==lastc)
      __asm __volatile("");
    acquire(&l);
  }
  release(&l);
  printf(1, "thrtest[%d]: done\n", getpid());

  for(int i = 0; i < nthread; i++)
    wait();
  exit();
}
