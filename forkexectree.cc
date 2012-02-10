#include "types.h"
#include "stat.h"
#include "user.h"
#include "mtrace.h"

#define NCHILD 2
#define NDEPTH 7

void
forktree(int depth)
{
  if (depth == 0) {
    printf(1, "%d: forkexectree\n", getpid());
    mtenable("xv6-forkexectree");
  }

  if (depth >= NDEPTH)
    exit();

  for (int i = 0; i < NCHILD; i++) {
    int pid = fork(0);
    if (pid < 0) {
      printf(1, "fork error\n");
      exit();
    }

    if (pid == 0) {
      depth++;
      char depthbuf[16];
      snprintf(depthbuf, sizeof(depthbuf), "%d", depth);
      const char *av[] = { "forkexectree", depthbuf };
      exec("forkexectree", av);
    }
  }

  for (int i = 0; i < NCHILD; i++) {
    if (wait() < 0) {
      printf(1, "wait stopped early\n");
      exit();
    }
  }
  
  if (wait() != -1) {
    printf(1, "wait got too many\n");
    exit();
  }

  if (depth > 0)
    exit();

  mtops(0);
  mtdisable("xv6-forkexectree");
  
  printf(1, "%d: forkexectree OK\n", getpid());
  // halt();
}

int
main(int ac, char **av)
{
  if (ac == 1) {
    forktree(0);
  } else {
    forktree(atoi(av[1]));
  }
  exit();
}
