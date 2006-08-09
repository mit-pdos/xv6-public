#include "user.h"
#include "types.h"
#include "fs.h"

// file system tests

char buf[1024];
char *echo_args[] = { "echo", "hello", "goodbye", 0 };
char *cat_args[] = { "cat", "README", 0 };

int
main(void)
{
  int fd;

  puts("userfs running\n");
  block();

  if (mknod ("console", T_DEV, 1, 1) < 0)
    puts ("mknod failed\n");
  else
    puts ("made a node\n");
  fd = open("console", 1);
  if(fd >= 0){
    puts("open console ok\n");
    close(fd);
  } else {
    puts("open console failed!\n");
  }
  if (write (fd, "hello\n", 6) != 6) {
    puts ("write to console failed\n");
  }
  close (fd);

  fd = open("echo", 0);
  if(fd >= 0){
    puts("open echo ok\n");
    close(fd);
  } else {
    puts("open echo failed!\n");
  }
  fd = open("doesnotexist", 0);
  if(fd >= 0){
    puts("open doesnotexist succeeded!\n");
    close(fd);
  } else {
    puts("open doesnotexist failed\n");
  }
  //exec("echo", echo_args);
  exec("cat", cat_args);
  return 0;
}
