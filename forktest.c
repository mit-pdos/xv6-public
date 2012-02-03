// Test that fork fails gracefully.
// Tiny executable so that the limit can be filling the proc table.

#include "types.h"
#include "stat.h"
#include "user.h"
#include "mtrace.h"

#define N 100
#define DEBUG 0

void
printf(int fd, char *s, ...)
{
  write(fd, s, strlen(s));
}

void
forktest(void)
{
  struct mtrace_appdata_entry entry;
  int n, pid;

  printf(1, "fork test\n");

  mtrace_enable_set(1, "xv6-forktest");
  for(n=0; n<N; n++){
    pid = fork(0);
    if(pid < 0)
      break;
    if(pid == 0)
      exit();
  }
  
  if(DEBUG && n == N){
    printf(1, "fork claimed to work N times!\n", N);
    exit();
  }
  
  for(; n > 0; n--){
    if(wait() < 0){
      printf(1, "wait stopped early\n");
      exit();
    }
  }
  
  if(wait() != -1){
    printf(1, "wait got too many\n");
    exit();
  }

  entry.u64 = n;
  mtrace_appdata_register(&entry);
  mtrace_enable_set(0, "xv6-forktest");
  
  printf(1, "fork test OK\n");
  // halt();
}

int
main(void)
{
  forktest();
  exit();
}
