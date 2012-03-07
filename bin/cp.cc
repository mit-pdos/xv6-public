#include "types.h"
#include "user.h"
#include "fcntl.h"

int
main(int argc, char *argv[])
{
  if(argc != 3){
    printf("Usage: cp fromfile tofile\n");
    exit();
  }

  int fd1 = open(argv[1], 0);
  if(fd1 < 0){
    printf("cp: cannot open %s\n", argv[1]);
    exit();
  }

  int fd2 = open(argv[2], O_CREATE|O_WRONLY);
  if(fd2 < 0){
    printf("cp: cannot create %s\n", argv[2]);
    exit();
  }

  int n;
  char buf[512];
  while((n = read(fd1, buf, sizeof(buf))) > 0){
    if(write(fd2, buf, n) != n){
      printf("cp: write failed\n");
      exit();
    }
  }

  exit();
}
