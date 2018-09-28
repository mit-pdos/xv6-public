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

void testfile(char *path) {
  int fd;
  struct stat st;
  if((fd = open(path, O_CREATE)) < 0){
    printf(2, "testfile: cannot open %s\n", path);
    return;
  }

  if (write(fd, "aaa", 3) < 0) {
    printf(2, "testfile: failed writing\n", path);
    close(fd);
    return;
  }

  close(fd);

  if((fd = open(path, 0)) < 0){
    printf(2, "testfile: cannot open %s\n", path);
    return;
  }

  if(fstat(fd, &st) < 0){
    printf(2, "testfile: cannot stat %s\n", path);
    close(fd);
    return;
  }

  printf(1, "testfile %s: %d %d %d\n", path, st.type, st.ino, st.size);

  if (st.size != 3) {
    printf(2, "testfile: incorrect length (%d) for file %s\n", st.size, path);
    close(fd);
    return;
  }

  char buf[4];
  int res;
  if ((res = read(fd, buf, 3)) != 3) {
    printf(2, "testfile: incorrect length read (%d) for file %s\n", res, path);
    close(fd);
    return;
  }
  buf[3] = '\0';
  close(fd);

  if ((res = strcmp("aaa", buf)) != 0) {
    printf(2, "testfile: incorrect content read (%s) for file %s\n", buf, path); 
    return;
  }

  printf(2, "testfile: SUCCESS", buf, path); 
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

  testfile("a/test1");

  printmounts();
  printdevices();
  res = umount("a");
  printf(1, "Result: umount returned %d\n", res);
  printmounts();
  printdevices();
  return 0;
}