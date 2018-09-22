#include "param.h"
#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "fcntl.h"
#include "syscall.h"
#include "traps.h"
#include "memlayout.h"

int
main(int argc, char *argv[])
{
  printf(1, "Mounttests starting\n");
  printmounts();
  printdevices();
  mkdir("a");
  int res = mount("README", "a");
  printf(1, "Result: mount returned %d\n", res);
  printmounts();
  printdevices();
  res = umount("a");
  printf(1, "Result: umount returned %d\n", res);
  printmounts();
  printdevices();
  return 0;
}