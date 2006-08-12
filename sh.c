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
    puts("$ ");
    gets(buf, sizeof(buf));
    if(buf[0] == '\0')
      continue;
    pid = fork();
    if(pid == 0){
      args[0] = buf;
      args[1] = 0;
      exec(buf, args);
      printf(1, "%s: not found\n", buf);
      exit();
    }
    if(pid > 0)
      wait();
  }
}
