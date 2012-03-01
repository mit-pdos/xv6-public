#include "types.h"
#include "stat.h"
#include "user.h"
#include "mtrace.h"
#include "amd64.h"
#include "uspinlock.h"
#include "pthread.h"

static volatile char *p;
static struct uspinlock l;
static volatile int state;

static void
spin(void)
{
  volatile int i;
  for (i = 0; i < 10000; i++)
    ;
}

void*
thr(void*)
{
  for (;;) {
    acquire(&l);
    if (state == 1) {
      p[0] = 'x';
      p[4096] = 'y';
      state = 2;
    }

    if (state == 3) {
      state = 4;
      fprintf(1, "about to access after unmap\n");
      release(&l);

      p[0] = 'X';
      p[4096] = 'Y';

      acquire(&l);
      fprintf(1, "still alive after unmap write\n");
      exit();
    }
    release(&l);
    spin();
  }
}

int
main(void)
{
  p = (char *) 0x80000;
  if (map((void *) p, 8192) < 0) {
    fprintf(1, "map failed\n");
    exit();
  }

  pthread_t tid;
  pthread_create(&tid, 0, thr, 0);

  acquire(&l);
  state = 1;
  while (state != 2) {
    release(&l);
    spin();
    acquire(&l);
  }

  if (p[0] != 'x' || p[4096] != 'y') {
    fprintf(1, "mismatch\n");
    exit();
  }

  fprintf(1, "shm ok\n");

  if (unmap((void *) p, 8192) < 0) {
    fprintf(1, "unmap failed\n");
    exit();
  }

  state = 3;
  fprintf(1, "waiting for unmap access\n");
  while (state != 4) {
    release(&l);
    spin();
    acquire(&l);
  }

  fprintf(1, "maptest done\n");
  exit();
}
