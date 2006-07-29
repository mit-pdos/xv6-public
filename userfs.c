#include "user.h"

// file system tests

char buf[1024];
char *args[] = { "echo", "hello", "goodbye", 0 };

int
main(void)
{
  int fd;

  puts("userfs running\n");
  block();
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
  exec("echo", args);
  return 0;
}
