#include "types.h"
#include "stat.h"
#include "user.h"
#include "xv6-mtrace.h"

#define NCHILD 2
#define NDEPTH 5

char*
strncpy(char *s, const char *t, int n)
{
  int tlen = strlen((char *)t);
  memmove(s, (char *)t, n > tlen ? tlen : n);
  if (n > tlen)
    s[tlen] = 0;
  return s;
}

void*
memcpy(void *dst, const void *src, uint n)
{
  return memmove(dst, (void *)src, n);
}

void
forktree(void)
{
  uint depth = 0;

  printf(1, "%d: fork tree\n", getpid());
  mtrace_enable_set(1, "xv6-forktree");

 next_level:
  //printf(1, "pid %d, depth %d\n", getpid(), depth);
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
      goto next_level;
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
  
  printf(1, "%d: fork tree OK\n", getpid());
  halt();
}

int
main(void)
{
  forktree();
  exit();
}
