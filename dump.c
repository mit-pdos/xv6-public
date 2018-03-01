#include "types.h"
#include "stat.h"
#include "user.h"
#include "syscall.h"

int
main(void){
  
  int pid = fork();
  if(pid != 0){
    printf(1,"sending pid: %d\n", pid);
    printf(1, "this should be pid: %d\n", dump(pid));
  }
  exit();
}
