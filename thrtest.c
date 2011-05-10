#include "types.h"
#include "stat.h"
#include "user.h"
#include "xv6-mtrace.h"
#include "x86.h"
#include "uspinlock.h"

static struct uspinlock l;
static volatile uint tcount;
enum { nthread = 1 };

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
  printf(1, "thrtest[%d]: start\n", getpid());

  for(uint i = 0; i < nthread; i++) {
    sbrk(4096);
    uint *tstack = (uint*) sbrk(0);
    tstack[-1] = 0xc0ffee00 | i;

    int tid = forkt(&tstack[-2], thr);
    printf(1, "thrtest[%d]: child %d esp %x\n", getpid(), tid, resp());
  }

  release(&l);
  exit();

  do{
    printf(1, "thrtest[%d]: %d\n", getpid(), tcount);
    for(uint i = 0; i < 100000; i++)
      ;
    //sleep(1);
  }while(tcount < nthread);
}
