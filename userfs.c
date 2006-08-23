#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "fcntl.h"

// file system tests

char buf[2000];
char name[3];
char *echo_args[] = { "echo", "hello", "goodbye", 0 };
char *cat_args[] = { "cat", "readme", 0 };

int
main(void)
{
  int fd;
  int i;
  int stdout = 1;

  printf(stdout, "userfs is running\n");

  if (sbrk(4096) < 0) {
    printf(stdout, "sbrk failed\n");
  }

  fd = open("echo", 0);
  if(fd >= 0){
    printf(stdout, "open echo ok\n");
    close(fd);
  } else {
    printf(stdout, "open echo failed!\n");
  }
  fd = open("doesnotexist", 0);
  if(fd >= 0){
    printf(stdout, "open doesnotexist succeeded!\n");
    close(fd);
  } else {
    printf(stdout, "open doesnotexist failed\n");
  }
  fd = open("doesnotexist", O_CREATE|O_RDWR);
  if(fd >= 0){
    printf(stdout, "creat doesnotexist succeeded\n");
  } else {
    printf(stdout, "error: creat doesnotexist failed!\n");
  }
  for (i = 0; i < 100; i++) {
    if (write (fd, "aaaaaaaaaa", 10) != 10) {
      printf(stdout, "error: write new file failed\n");
    }
    if (write (fd, "bbbbbbbbbb", 10) != 10) {
      printf(stdout, "error: write new file failed\n");
    }
  }
  printf(stdout, "writes done\n");
  close(fd);
  fd = open("doesnotexist", O_RDONLY);
  if(fd >= 0){
    printf(stdout, "open doesnotexist succeeded\n");
  } else {
    printf(stdout, "error: open doesnotexist failed!\n");
  }
  i = read(fd, buf, 2000);
  if (i == 2000) {
    printf(stdout, "read succeeded\n");
  } else {
    printf(stdout, "read failed\n");
  }
  close(fd);

  printf(stdout, "unlink doesnotexist\n");

  unlink("doesnotexist");

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

  printf(stdout, "mkdir\n");

  if (mkdir("dir0") < 0) 
    printf(stdout, "mkdir failed\n");

  //  unlink("dir0");

  //exec("echo", echo_args);
  printf(stdout, "about to do exec\n");
  exec("cat", cat_args);
  return 0;
}
