#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "fcntl.h"

char *args[100];
void parse(char buf[]);

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
      parse(buf);
      exec(buf, args);
      printf(1, "%s: not found\n", buf);
      exit();
    }
    if(pid > 0)
      wait();
  }
}

void
parse(char buf[])
{
  int j = 1;
  int i;
  args[0] = buf;
  for (i = 0; buf[i] != '\0'; i++) {
    if (buf[i] == ' ') {
      buf[i] = '\0';
      args[j++] = buf + i+1;
      if (j >= 100) {
	printf(2, "too many args\n");
	exit();
      }
    }
  }
}
