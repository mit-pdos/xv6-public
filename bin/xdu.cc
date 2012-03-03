#if defined(LINUX)
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <stdint.h>
#include <stddef.h>
#include <errno.h>
#define ST_SIZE(st)  (st).st_size
#define ST_ISDIR(st) S_ISDIR((st).st_mode)
#define BSIZ 256
typedef DIR* dir_t;

static inline int getdent(dir_t d, char *buf, size_t n)
{
  struct dirent *entryp, *result;
  char *dbuf[512];
  int r;

  entryp = (struct dirent *)dbuf;
  r = readdir_r(d, entryp, &result);
  if (r == 0 && result != NULL) {
    strcpy(buf, entryp->d_name);
    return 0;
  }
  return -1;
}

#else // assume xv6

#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#define ST_SIZE(st)  (st).size
#define ST_ISDIR(st) ((st).type == T_DIR)
#define stderr 2
#define BSIZ (DIRSIZ+1)
typedef int dir_t;

static inline dir_t 
fdopendir(int fd) {
  return fd;
}

static inline void
closedir(dir_t d) {

}

static inline int 
getdent(dir_t fd, char *buf, size_t n)
{
  struct dirent de;
  int r; 

again:
  r = read(fd, &de, sizeof(de));
  if (r == sizeof(de)) {
    if (de.inum == 0)
      goto again;

    memmove(buf, de.name, n-1);
    buf[n-1] = 0;
    return 0;
  }

  return -1;
}
#endif

static int
du(int fd)
{
  struct stat st;
  if (fstat(fd, &st) < 0) {
    fprintf(stderr, "du: cannot stat\n");
    close(fd);
    return 0;
  }

  int size = ST_SIZE(st);
  if (ST_ISDIR(st)) {
    char buf[BSIZ];
    dir_t d = fdopendir(fd);
    while (getdent(d, buf, BSIZ) == 0) {
      if (!strcmp(buf, ".") || !strcmp(buf, ".."))
        continue;

      int nfd = openat(fd, buf, 0);
      if (nfd >= 0)
        size += du(nfd);  // should go into work queue
    }
    closedir(d);
  }

  close(fd);
  return size;
}

int
main(int ac, char **av)
{
  printf("%d\n", du(open(".", 0)));
  return 0;
}
