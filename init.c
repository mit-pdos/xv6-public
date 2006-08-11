#include "user.h"
#include "types.h"
#include "fs.h"
#include "fcntl.h"

char *sh_args[] = { "sh", 0 };

int
main(void)
{
  int pid;
  
  if(open("console", 0) < 0){
    mknod("console", T_DEV, 1, 1);
    open("console", 0);
  }
  open("console", 1);
  open("console", 1);

  write(1, "init...\n", 8);

  while(1){
    write(1, "running sh...\n", 14);
    pid = fork();
    if(pid == 0){
      exec("sh", sh_args);
      exit();
    }
    if(pid > 0)
      wait();
  }
}
