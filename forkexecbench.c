#include "types.h"
#include "stat.h"
#include "user.h"
#include "mtrace.h"
#include "amd64.h"

#define NITERS 64

static void
execbench(void)
{
  u64 s = rdtsc();
  for (int i = 0; i < NITERS; i++) {
    int pid = fork(0);
    if (pid < 0) {
      printf(1, "fork error\n");
      exit();
    }
    if (pid == 0) {
      char *av[] = { "forkexecbench", "x", 0 };
      exec("forkexecbench", av);
      printf(1, "exec failed\n");
      exit();
    } else {
      wait();
    }
  }
  u64 e = rdtsc();
  printf(1, "%lu\n", (e-s) / NITERS);
}

int
main(int ac, char **av)
{
  if (ac == 2)
      exit();
  execbench();
  exit();
}
