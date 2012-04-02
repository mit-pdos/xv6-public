#include "types.h"
#include "stat.h"
#include "user.h"

#define NCHILD 2
#define NDEPTH 12

void
forktree(int depth)
{
  if (depth == 0) {
    fprintf(1, "%d: forkexectree\n", getpid());
  }

  if (depth >= NDEPTH)
    exit();

  for (int i = 0; i < NCHILD; i++) {
    int pid = fork(0);
    if (pid < 0) {
      fprintf(1, "fork error\n");
      exit();
    }

    if (pid == 0) {
      depth++;
      char depthbuf[16];
      snprintf(depthbuf, sizeof(depthbuf), "%d", depth);
      const char *av[] = { "forkexectree", depthbuf, 0 };
      int r = exec("forkexectree", av);
      fprintf(1, "forkexectree: exec failed %d\n", r);
      exit();
    }
  }

  for (int i = 0; i < NCHILD; i++) {
    if (wait() < 0) {
      fprintf(1, "wait stopped early\n");
      exit();
    }
  }
  
  if (wait() != -1) {
    fprintf(1, "wait got too many\n");
    exit();
  }

  if (depth > 0)
    exit();

  fprintf(1, "%d: forkexectree OK\n", getpid());
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
