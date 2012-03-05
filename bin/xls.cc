#if defined(LINUX)
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#define ST_SIZE(st)  (st).st_size
#define ST_TYPE(st)  0
#define ST_INO(st)   (st).st_ino
#define ST_ISDIR(st) S_ISDIR((st).st_mode)
#define ST_ISREG(st) S_ISREG((st).st_mode)
#define BSIZ 256
#else // assume xv6
#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#define ST_SIZE(st)  (st).size
#define ST_TYPE(st)  (st).type
#define ST_INO(st)   (st).ino
#define ST_ISDIR(st) ((st).type == T_DIR)
#define ST_ISREG(st) ((st).type == T_FILE)
#define BSIZE (DIRSIZ + 1)
#define stderr 2
#endif

const char*
fmtname(const char *path)
{
  static char buf[BSIZ];
  const char *p;
  
  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;
  
  // Return blank-padded name.
  if(strlen(p) >= BSIZ-1)
    return p;
  memmove(buf, p, strlen(p));
  memset(buf+strlen(p), ' ', BSIZ-1-strlen(p));
  return buf;
}

void
ls(const char *path)
{
  char buf[512], *p;
  int fd;
  struct dirent de;
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
    printf("%s %d %d %d\n", fmtname(path), ST_TYPE(st), ST_INO(st), ST_SIZE(st));
  } else if (ST_ISDIR(st)) {
    if(strlen(path) + 1 + BSIZ > sizeof buf) {
      printf("ls: path too long\n");
    }
    strcpy(buf, path);
    p = buf+strlen(buf);
    *p++ = '/';
    while(read(fd, &de, sizeof(de)) == sizeof(de)){
      if(de.inum == 0)
        continue;
      memmove(p, de.name, BSIZ-1);
      p[BSIZ-1] = 0;
      if(stat(buf, &st) < 0){
        printf("ls: cannot stat %s\n", buf);
        continue;
      }
      printf("%s %d %d %d\n", fmtname(buf), ST_TYPE(st), ST_INO(st), ST_SIZE(st));
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
