#include "types.h"
#include "stat.h"
#include "user.h"

#define NCHILD 2
#define NDEPTH 5

void
forktree(void)
{
  int depth = 0;

  fprintf(1, "%d: fork tree\n", getpid());

 next_level:
  //printf(1, "pid %d, depth %d\n", getpid(), depth);
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
      goto next_level;
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

  fprintf(1, "%d: fork tree OK\n", getpid());
  // halt();
}

int
main(void)
{
  forktree();
  exit();
}
