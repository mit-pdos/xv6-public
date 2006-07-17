#include "user.h"

char buf[32];

int
main(void)
{
  int pid, fds[2], n;

  pipe(fds);
  pid = fork();
  if(pid > 0){
    write(fds[1], "xyz", 4);
    puts("w");
  } else {
    n = read(fds[0], buf, sizeof(buf));
    puts("r: ");
    puts(buf);
    puts("\n");
  }
  for(;;);
}
