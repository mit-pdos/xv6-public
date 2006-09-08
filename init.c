#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "fcntl.h"

// init: The initial user-level program

char *sh_args[] = { "sh", 0 };

int
main(void)
{
  int pid;

  if(open("console", O_RDWR) < 0){
    mknod("console", T_DEV, 1, 1);
    open("console", O_RDWR);
  }
  dup(0);  // stdout
  dup(0);  // stderr

  for(;;){
    pid = fork();
    if(pid < 0){
      puts("init: fork failed\n");
      exit();
    }
    if(pid == 0){
      exec("sh", sh_args);
      puts("init: exec sh failed\n");
      exit();
    } else {
      wait();
    }
  }
}
