// Test that fork fails gracefully.
// Tiny executable so that the limit can be filling the proc table.

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define N  1000

void
print(const char *s)
{
  write(1, s, strlen(s));
}

void
forktest(void)
{
  int n, pid;

  print("fork test\n");

  for(n=0; n<N; n++){
    pid = fork();
    if(pid < 0)
      break;
    if(pid == 0)
      exit();
  }

  if(n == N){
    print("fork claimed to work N times!\n");
    exit();
  }

  for(; n > 0; n--){
    if(wait() < 0){
      print("wait stopped early\n");
      exit();
    }
  }

  if(wait() != -1){
    print("wait got too many\n");
    exit();
  }

  print("fork test OK\n");
}

int
main(void)
{
  forktest();
  exit();
}
