#if defined(LINUX)
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <errno.h>
#include "include/wq.hh"
#include "user/dirit.hh"
#define ST_SIZE(st)  (st).st_size
#define ST_ISDIR(st) S_ISDIR((st).st_mode)
#define BSIZ 256

#else // assume xv6

#include "types.h"
#include "stat.h"
#include "user.h"
#include "lib.h"
#include "fs.h"
#include "wq.hh"
#include "dirit.hh"
#define ST_SIZE(st)  (st).size
#define ST_ISDIR(st) ((st).type == T_DIR)
#define stderr 2
#define BSIZ (DIRSIZ+1)
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
    dirit di(fd);

    wq_for<dirit>(di,
                  [](dirit &i)->bool { return !i.end(); },
                  [&](dirit &i)->void
    {
      char buf[BSIZ];
      i.name(buf, BSIZ);
      if (!strcmp(buf, ".") || !strcmp(buf, ".."))
          return;

      int nfd = openat(fd, buf, 0);
      if (nfd >= 0)
        size += du(nfd);  // should go into work queue
    });
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
