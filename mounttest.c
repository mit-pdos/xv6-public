#include "param.h"
#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "fcntl.h"
#include "syscall.h"
#include "traps.h"
#include "memlayout.h"

void fstat_file(char *path, struct stat *st) {
  int fd;
  if((fd = open(path, 0)) < 0){
    printf(1, "mounttest: cannot open %s\n", path);
    return;
  }

  if(fstat(fd, st) < 0){
    printf(1, "mounttest: cannot stat %s\n", path);
    close(fd);
    return;
  }

  close(fd);
}

void testfile(char *path) {
  int fd;
  struct stat st;
  if((fd = open(path, O_WRONLY|O_CREATE)) < 0){
    printf(1, "testfile: cannot open %s\n", path);
    return;
  }

  if (write(fd, "aaa", 3) < 0) {
    printf(1, "testfile: failed writing\n", path);
    close(fd);
    return;
  }

  close(fd);

  if((fd = open(path, 0)) < 0){
    printf(1, "testfile: cannot open %s\n", path);
    return;
  }

  if(fstat(fd, &st) < 0){
    printf(1, "testfile: cannot stat %s\n", path);
    close(fd);
    return;
  }

  if (st.size != 3) {
    printf(1, "testfile: incorrect length (%d) for file %s\n", st.size, path);
    close(fd);
    return;
  }

  char buf[4];
  int res;
  if ((res = read(fd, buf, 3)) != 3) {
    printf(1, "testfile: incorrect length read (%d) for file %s\n", res, path);
    close(fd);
    return;
  }
  buf[3] = '\0';
  close(fd);

  if ((res = strcmp("aaa", buf)) != 0) {
    printf(1, "testfile: incorrect content read (%s) for file %s\n", buf, path); 
    return;
  }

  printf(1, "testfile: SUCCESS\n", buf, path); 
}

int mounta() {
  mkdir("a");
  int res = mount("internal_fs_a", "a");
  if (res != 0) {
    printf(1, "mounta: mount returned %d\n", res);
    return -1;
  }

  return 0;
}

int umounta() {
  int res = umount("a");
  if (res != 0) {
    printf(1, "umounta: umount returned %d\n", res);
    printmounts();
    printdevices();
    return -1;
  }

  return 0;
}

void mounttest() {
  if (mounta() != 0) {
    return;
  }
  
  if (umounta() != 0) {
    return;
  }

  printf(1, "mounttest: SUCCESS\n");
}

void statroottest() {
  if (mounta() != 0) {
    return;
  }

  struct stat st;
  fstat_file("a", &st);
  if (st.type != T_DIR || st.ino != 1 || st.size != BSIZE) {
    printf(1, "statroottest: FAILED - %d %d %d\n", st.type, st.ino, st.size);
    return;
  }

  if (umounta() != 0) {
    return;
  }

  printf(1, "statroottest: SUCCESS\n");
}

void writefiletest() {
  if (mounta() != 0) {
    return;
  }

  testfile("a/test1");

  if (umounta() != 0) {
    return;
  }
}

void invalidpathtest() {
  int res = mount("internal_fs_a", "AAA");
  if (res != -1) {
    printf(1, "invalidpathtest: mount did not fail as expected %d\n", res);
    return;
  }

  if (mounta() != 0) {
    return;
  }

  res = umount("b");
  if (res != -1) {
    printf(1, "invalidpathtest: umount did not fail as expected %d\n", res);
    return;
  }

  mkdir("b");
  res = umount("b");
  if (res != -1) {
    printf(1, "invalidpathtest: umount did not fail as expected %d\n", res);
    return;
  }

  if (umounta() != 0) {
    return;
  }

  printf(1, "invalidpathtest: SUCCESS\n");
}

void doublemounttest() {
  if (mounta() != 0) {
    return;
  }

  mkdir("b");
  int res = mount("internal_fs_a", "b");
  if (res != 0) {
    printf(1, "doublemounttest: mount returned %d\n", res);
    return;
  }

  if (umounta() != 0) {
    return;
  }

  res = umount("b");
  if (res != 0) {
    printf(1, "doublemounttest: umount returned %d\n", res);
    printmounts();
    printdevices();
    return;
  }


  printf(1, "doublemounttest: SUCCESS\n");
}

void samedirectorytest() {
  if (mounta() != 0) {
    return;
  }

  int res = mount("internal_fs_b", "a");
  if (res != -1) {
    printf(1, "samedirectorytest: mount did not fail as expected %d\n", res);
    return;
  }

  if (umounta() != 0) {
    return;
  }

  printf(1, "samedirectorytest: SUCCESS\n");
}

int
main(int argc, char *argv[])
{
  printf(1, "Mounttests starting...\n");
  mounttest();
  statroottest();
  writefiletest();
  invalidpathtest();
  doublemounttest();
  samedirectorytest();

  unlink("a");
  unlink("b");
  return 0;
}