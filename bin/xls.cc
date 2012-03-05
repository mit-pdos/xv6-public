#if defined(LINUX)
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include "user/dirit.hh"
#define ST_SIZE(st)  (st).st_size
#define ST_TYPE(st)  (ST_ISDIR(st) ? 1 : ST_ISREG(st) ? 2 : 3)
#define ST_INO(st)   (st).st_ino
#define ST_ISDIR(st) S_ISDIR((st).st_mode)
#define ST_ISREG(st) S_ISREG((st).st_mode)
#define BSIZ 256
#else // assume xv6
#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "lib.h"
#include "dirit.hh"
#define ST_SIZE(st)  (st).size
#define ST_TYPE(st)  (st).type
#define ST_INO(st)   (st).ino
#define ST_ISDIR(st) ((st).type == T_DIR)
#define ST_ISREG(st) ((st).type == T_FILE)
#define BSIZ (DIRSIZ + 1)
#define stderr 2
#endif

void
ls(const char *path)
{
  char buf[512], *p;
  int fd;
  struct stat st;
  
  if((fd = open(path, 0)) < 0){
    fprintf(stderr, "ls: cannot open %s\n", path);
    return;
  }
  
  if(fstat(fd, &st) < 0){
    fprintf(stderr, "ls: cannot stat %s\n", path);
    close(fd);
    return;
  }
  
  if (ST_ISREG(st)) {
    printf("%u %10lu %10lu %s\n", ST_TYPE(st), ST_INO(st), ST_SIZE(st), path);
  } else if (ST_ISDIR(st)) {
    if(strlen(path) + 1 + BSIZ > sizeof buf) {
      printf("ls: path too long\n");
    }
    strcpy(buf, path);
    p = buf+strlen(buf);
    *p++ = '/';
    dirit di(fd);
    for (; !di.end(); ++di) {
      const char *name = *di;
      size_t len = strlen(name);
      memmove(p, name, len);
      p[len] = 0;
      if (stat(buf, &st) < 0){
        free((void*)name);
        printf("ls: cannot stat %s\n", buf);
        continue;
      }

      printf("%u %10lu %10lu %s\n", ST_TYPE(st), ST_INO(st), ST_SIZE(st), name);
      free((void*)name);
    }
  }
  close(fd);
}

int
main(int argc, char *argv[])
{
  int i;

  if(argc < 2){
    ls(".");
    return 0;
  }
  for(i=1; i<argc; i++)
    ls(argv[i]);
  return 0;
}
