// Test that fork fails gracefully.
// Tiny executable so that the limit can be filling the proc table.

#include "types.h"
#include "stat.h"
#include "user.h"

#define N  1000

void
printf(int fd, const char *s, ...)
{
  write(fd, s, strlen(s));
}

void
forktest(void)
{
  int n, pid;

  printf(1, "fork test\n");

  for(n=0; n<N; n++){
    pid = fork();
    if(pid < 0)
      break;
    if(pid == 0)
      exit(0);
  }

  if(n == N){
    printf(1, "fork claimed to work N times!\n", N);
    exit(1);
  }

  for(; n > 0; n--){
    if(wait() < 0){
      printf(1, "wait stopped early\n");
      exit(1);
    }
  }

  if(wait() != -1){
    printf(1, "wait got too many\n");
    exit(1);
  }

  printf(1, "fork test OK\n");
}

int
main(void)
{
  forktest();
  exit(0);
}
