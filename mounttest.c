#include "param.h"
#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "fcntl.h"
#include "syscall.h"
#include "traps.h"
#include "memlayout.h"

void fstat_file(char *path) {
  int fd;
  struct stat st;
  if((fd = open(path, 0)) < 0){
    printf(2, "mounttest: cannot open %s\n", path);
    return;
  }

  if(fstat(fd, &st) < 0){
    printf(2, "mounttest: cannot stat %s\n", path);
    close(fd);
    return;
  }

  printf(1, "fstat %s: %d %d %d\n", path, st.type, st.ino, st.size);

  close(fd);
}

int
main(int argc, char *argv[])
{
  printf(1, "Mounttests starting\n");
  printmounts();
  printdevices();
  mkdir("a");
  int res = mount("internal_fs_a", "a");
  printf(1, "Result: mount returned %d\n", res);
  fstat_file("a");
  printmounts();
  printdevices();
  res = umount("a");
  printf(1, "Result: umount returned %d\n", res);
  printmounts();
  printdevices();
  return 0;
}