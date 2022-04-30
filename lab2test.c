#include "types.h"
#include "user.h"

void work() {
  int i,k;
  for(i = 0; i < 43000; i++) {
    for(k = 0; k < 43000; k++) {
      asm("nop");
    }
  }
}

int main() {
  setprior(0);
  int i = 0;
  int pid = 0;
  for(i = 0; i < 3; i++) {
    pid = fork();
    if(!pid) {
      setprior(i * 10);
      work();
      printf(0, "child %d done\n", getpid());
      exit();
    }
  }
  if(pid) {
    for(i = 0; i < 3; i++)
      wait();
  }
  printf(0, "parent %d done\n", getpid());
  exit();
}
