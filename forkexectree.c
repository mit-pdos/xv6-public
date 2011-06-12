#include "types.h"
#include "stat.h"
#include "user.h"
#include "xv6-mtrace.h"

#define NCHILD 2
#define NDEPTH 7

void
forktree(uint depth)
{
  if (depth == 0) {
    printf(1, "%d: forkexectree\n", getpid());
    mtrace_enable_set(1, "xv6-forktree");
  }

  if (depth >= NDEPTH)
    exit();

  for (uint i = 0; i < NCHILD; i++) {
    int pid = fork(0);
    if (pid < 0) {
      printf(1, "fork error\n");
      exit();
    }

    if (pid == 0) {
      depth++;
      char depthbuf[16];
      snprintf(depthbuf, sizeof(depthbuf), "%d", depth);
      char *av[] = { "forkexectree", depthbuf };
      exec("forkexectree", av);
    }
  }

  for (uint i = 0; i < NCHILD; i++) {
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

  struct mtrace_appdata_entry entry;
  entry.u64 = 0;
  mtrace_appdata_register(&entry);
  mtrace_enable_set(0, "xv6-forktree");
  
  printf(1, "%d: forkexectree OK\n", getpid());
  halt();
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
