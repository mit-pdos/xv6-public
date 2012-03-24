#if defined(LINUX)
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include "user/dirit.hh"
#include "wq.hh"
#define ST_SIZE(st)  (st).st_size
#define ST_TYPE(st)  (ST_ISDIR(st) ? 1 : ST_ISREG(st) ? 2 : 3)
#define ST_INO(st)   (st).st_ino
#define ST_ISDIR(st) S_ISDIR((st).st_mode)
#define ST_ISREG(st) S_ISREG((st).st_mode)
#define BSIZ 256
#define xfstatat(fd, n, st) fstatat(fd, n, st, 0)
#define perf_stop() do { } while(0)
#define perf_start(x, y) do { } while (0)
#else // assume xv6
#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "lib.h"
#include "dirit.hh"
#include "wq.hh"
#define ST_SIZE(st)  (st).size
#define ST_TYPE(st)  (st).type
#define ST_INO(st)   (st).ino
#define ST_ISDIR(st) ((st).type == T_DIR)
#define ST_ISREG(st) ((st).type == T_FILE)
#define BSIZ (DIRSIZ + 1)
#define stderr 2
#define xfstatat fstatat
#endif

static const bool silent = false;

void
ls(const char *path)
{
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
    if (!silent)
      printf("%u %10lu %10lu %s\n",
             ST_TYPE(st), ST_INO(st), ST_SIZE(st), path);
    close(fd);
  } else if (ST_ISDIR(st)) {
    dirit di(fd);
    wq_for<dirit>(di,
                  [](dirit &i)->bool { return !i.end(); },
                  [&fd](const char *name)->void
    {
      struct stat st;
      if (xfstatat(fd, name, &st) < 0){
        printf("ls: cannot stat %s\n", name);
        free((void*)name);
        return;
      }
      
      if (!silent)
        printf("%u %10lu %10lu %s\n",
               ST_TYPE(st), ST_INO(st), ST_SIZE(st), name);
      free((void*)name);
    });
  } else {
    close(fd);
  }
}

int
main(int argc, char *argv[])
{
  int i;

  initwq();

  perf_start(PERF_SELECTOR, 10000);
  if(argc < 2) {
    ls(".");
  } else {
    // XXX(sbw) wq_for
    for (i=1; i<argc; i++)
      ls(argv[i]);
  }
  perf_stop();
  
  wq_dump();
  return 0;
}
