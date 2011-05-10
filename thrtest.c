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
thr(uint arg)
{
  acquire(&l);
  printf(1, "thrtest[%d]: arg 0x%x esp %x\n", getpid(), arg, resp());
  tcount++;
  release(&l);
  exit();
}

int
main(void)
{
  acquire(&l);
  printf(1, "thrtest[%d]: start esp %x\n", getpid(), resp());

  for(uint i = 0; i < nthread; i++) {
    sbrk(4096);
    uint *tstack = (uint*) sbrk(0);
    tstack[-1] = 0xc0ffee00 | i;

    int tid = forkt(&tstack[-2], thr);
    printf(1, "thrtest[%d]: child %d\n", getpid(), tid);
  }

  for(;;){
    uint lastc = tcount;
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

  for(uint i = 0; i < nthread; i++)
    wait();
  exit();
}
