#include "types.h"
#include "stat.h"
#include "user.h"
#include "mtrace.h"
#include "amd64.h"

#define NITERS 100

#define PROCMAX NCPU
#define PROCMIN (NCPU-4)

static void
worker0(void)
{
  const char* av[] = { "exechack", "w", 0 };
  exec(av[0], av);
  die("worker exec");
}

static void
worker1(void)
{
  exit();
}

static void
master(void)
{
  u64 pcount = 0;
  u64 i = 0;

  u64 t0 = rdtsc();
  while (i < NITERS) {
    while (pcount < PROCMAX) {
      int pid;
      pid = fork(0);
      if (pid < 0)
        die("master fork");
      if (pid == 0)
        worker0();
      pcount++;
    }
    
    while (pcount > PROCMIN) {
      if (wait() < 0)
        die("master wait");
      pcount--;
      i++;
    }
  }

  while (pcount) {
    wait();
    pcount--;
  }
  u64 t1 = rdtsc();

  printf("%lu\n", (t1-t0)/i);
}

int
main(int ac, char **av)
{
  if (ac > 1 && av[1][0] == 'w')
    worker1();
  master();
}
