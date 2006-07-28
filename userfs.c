#include "user.h"

// file system tests

char buf[1024];
char *args[] = { "echo", "hello", "goodbye", 0 };

int
main(void)
{
  puts("userfs running\n");
  block();
  exec("echo", args);
  return 0;
}
