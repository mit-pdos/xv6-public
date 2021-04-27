// Test that fork fails gracefully.
// Tiny executable so that the limit can be filling the proc table.

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define N  1000

void
fprintf(int fd, const char *s, ...)
{
  write(fd, s, strlen(s));
}
void
printf(const char *fmt, ...)
{
  fprintf(1,fmt);
}

void
forktest(void)
{
  int n, pid;

  printf("fork test\n");

  for(n=0; n<N; n++){
    pid = fork();
    if(pid < 0)
      break;
    if(pid == 0)
      exit();
  }

  if(n == N){
    printf("fork claimed to work N times!\n", N);
    exit();
  }

  for(; n > 0; n--){
    if(wait() < 0){
      printf("wait stopped early\n");
      exit();
    }
  }

  if(wait() != -1){
    fprintf(2, "wait got too many\n");
    exit();
  }

  printf("fork test OK\n");
}

int
main(void)
{
  forktest();
  exit();
}
