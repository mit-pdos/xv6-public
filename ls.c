#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"

char buf[512];
struct stat stat;
struct dirent dirent;

int
main(int argc, char *argv[])
{
  int fd;
  uint off;

  if(argc > 1){
    puts("Usage: ls\n");
    exit();
  }

  fd = open(".", 0);
  if(fd < 0){
    printf(2, "ls: cannot open .\n");
    exit();
  }
  if (fstat(fd, &stat) < 0) {
    printf(2, "ls: cannot open .\n");
    exit();
  }
  if (stat.st_type != T_DIR) {
    printf(2, "ls: . is not a dir\n");
  }
  for(off = 0; off < stat.st_size; off += sizeof(struct dirent)) {
    if (read(fd, &dirent, sizeof(struct dirent)) != sizeof(struct dirent)) {
      printf(2, "ls: read error\n");
      exit();
    }
    if (dirent.inum != 0)
      printf(1, "%s\n", dirent.name);
  }
  close(fd);

  exit();
}
