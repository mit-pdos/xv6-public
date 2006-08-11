#include "user.h"
#include "types.h"
#include "fs.h"
#include "fcntl.h"

char *args[100];

int
main(void)
{
  char buf[128];
  int pid;

  while(1){
    write(1, "$ ", 2);
    gets(buf, sizeof(buf));
    if(buf[0] == '\0')
      continue;
    pid = fork();
    if(pid == 0){
      args[0] = buf;
      args[1] = 0;
      exec(buf, args);
      write(1, buf, strlen(buf));
      write(1, ": not found\n", 12);
      exit();
    }
    if(pid > 0)
      wait();
  }
}
