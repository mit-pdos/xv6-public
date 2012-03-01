#include "types.h"
#include "stat.h"
#include "user.h"
#include "mtrace.h"
#include "amd64.h"
#include "uspinlock.h"
#include "pthread.h"

static struct uspinlock l;
static volatile int tcount;
static pthread_key_t tkey;
static pthread_barrier_t bar;
enum { nthread = 8 };

void*
thr(void *arg)
{
  pthread_setspecific(tkey, arg);
  pthread_barrier_wait(&bar);

  acquire(&l);
  printf("thrtest[%d]: arg 0x%lx rsp %lx spec %p\n",
         getpid(), arg, rrsp(), pthread_getspecific(tkey));
  tcount++;
  release(&l);
  exit();
}

int
main(void)
{
  acquire(&l);
  fprintf(1, "thrtest[%d]: start esp %x\n", getpid(), rrsp());

  pthread_key_create(&tkey, 0);
  pthread_barrier_init(&bar, 0, nthread);

  for(int i = 0; i < nthread; i++) {
    pthread_t tid;
    pthread_create(&tid, 0, &thr, (void*) (0xc0ffee00ULL | i));
    fprintf(1, "thrtest[%d]: child %d\n", getpid(), tid);
  }

  for(;;){
    int lastc = tcount;
    fprintf(1, "thrtest[%d]: tcount=%d\n", getpid(), lastc);
    release(&l);
    if(lastc==nthread)
      break;
    while(tcount==lastc)
      __asm __volatile("");
    acquire(&l);
  }
  release(&l);
  fprintf(1, "thrtest[%d]: done\n", getpid());

  for(int i = 0; i < nthread; i++)
    wait();
  exit();
}
