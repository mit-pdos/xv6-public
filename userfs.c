#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "fcntl.h"

// simple file system tests

char buf[2000];
char name[3];
char *echo_args[] = { "echo", "hello", "goodbye", 0 };
char *cat_args[] = { "cat", "readme", 0 };
int stdout = 1;

void
opentest(void)
{
  int fd;

  fd = open("echo", 0);
  if(fd >= 0){
    printf(stdout, "open echo ok\n");
    close(fd);
  } else {
    printf(stdout, "open echo failed!\n");
    exit();
  }
  fd = open("doesnotexist", 0);
  if(fd >= 0){
    printf(stdout, "open doesnotexist succeeded!\n");
    exit();
  } else {
    printf(stdout, "open doesnotexist failed\n");
  }
}

void 
writetest(void)
{
  int fd;
  int i;

  fd = open("small", O_CREATE|O_RDWR);
  if(fd >= 0){
    printf(stdout, "creat small succeeded\n");
  } else {
    printf(stdout, "error: creat small failed!\n");
    exit();
  }
  for (i = 0; i < 100; i++) {
    if (write (fd, "aaaaaaaaaa", 10) != 10) {
      printf(stdout, "error: write aa %d new file failed\n", i);
      exit();
    }
    if (write (fd, "bbbbbbbbbb", 10) != 10) {
      printf(stdout, "error: write bb %d new file failed\n", i);
      exit();
    }
  }
  printf(stdout, "writes done\n");
  close(fd);
  fd = open("small", O_RDONLY);
  if(fd >= 0){
    printf(stdout, "open small succeeded\n");
  } else {
    printf(stdout, "error: open small failed!\n");
    exit();
  }
  i = read(fd, buf, 2000);
  if (i == 2000) {
    printf(stdout, "read succeeded\n");
  } else {
    printf(stdout, "read failed\n");
    exit();
  }
  close(fd);

  if (unlink("small") < 0) {
    printf(stdout, "unlink small failed\n");
    exit();
  }
}

void 
writetest1(void)
{
  int i, fd, n;

  printf(stdout, "big files\n");

  fd = open("big", O_CREATE|O_RDWR);
  if(fd < 0){
    printf(stdout, "error: creat big failed!\n");
    exit();
  }

  for (i = 0; i < MAXFILE; i++) {
    ((int *) buf)[0] = i;
    if (write (fd, buf, 512) != 512) {
      printf(stdout, "error: write big file failed\n", i);
      exit();
    }
  }

  close(fd);

  fd = open("big", O_RDONLY);
  if(fd < 0){
    printf(stdout, "error: open big failed!\n");
    exit();
  }

  n = 0;
  while (1) {
    i = read(fd, buf, 512);
    if (i == 0) {
      if (n == MAXFILE - 1) {
	printf(stdout, "read only %d blocks from big", n);
	exit();
      }
      break;
    } else if (i != 512) {
      printf(stdout, "read failed %d\n", i);
      exit();
    }
    if (((int *)buf)[0] != n) {
      printf(stdout, "read content of block %d is %d\n", n, ((int *)buf)[0]);
      exit();
    }
    n++;
  }
  close(fd);
  if (unlink("big") < 0) {
    printf(stdout, "unlink big failed\n");
    exit();
  }
}

void
createtest(void)
{
  int i, fd;

  printf(stdout, "many creates, followed by unlink\n");

  name[0] = 'a';
  name[2] = '\0';
  for (i = 0; i < 52; i++) {
    name[1] = '0' + i;
    fd = open(name, O_CREATE|O_RDWR);
    close(fd);
  }
  name[0] = 'a';
  name[2] = '\0';
  for (i = 0; i < 52; i++) {
    name[1] = '0' + i;
    unlink(name);
  }
}

void dirtest(void)
{
  printf(stdout, "mkdir\n");

  if (mkdir("dir0") < 0)  {
    printf(stdout, "mkdir failed\n");
    exit();
  }

  if (chdir("dir0") < 0)  {
    printf(stdout, "chdir dir0 failed\n");
    exit();
  }

  if (chdir("..") < 0)  {
    printf(stdout, "chdir .. failed\n");
    exit ();
  }
  
  if (unlink("dir0") < 0) {
    printf(stdout, "unlink dir0 failed\n");
    exit();
  }
}

void
exectest(void)
{
  if (exec("echo", echo_args) < 0) {
    printf(stdout, "exec echo failed\n");
    exit();
  }
  if (exec("cat", cat_args) < 0) {
    printf(stdout, "exec cat failed\n");
    exit();
  }
}

int
main(void)
{
  printf(stdout, "userfs is running\n");
  opentest();
  writetest();
  writetest1();
  createtest();
  exectest();
  return 0;
}
