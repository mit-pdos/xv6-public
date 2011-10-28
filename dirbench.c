#include "types.h"
#include "stat.h"
#include "user.h"
#include "mtrace.h"
#include "amd64.h"
#include "param.h"
#include "fcntl.h"

enum { nthread = 2 };
enum { nloop = 100 };
enum { nfile = 10 };
enum { nlookup = 100 };
char dirs[nthread][MAXNAME];

void
bench(uint tid)
{
  char pn[MAXNAME];

  for (uint x = 0; x < nloop; x++) {
    for (uint i = 0; i < nfile; i++) {
      snprintf(pn, sizeof(pn), "%s/f:%d:%d", dirs[tid], tid, i);

      int fd = open(pn, O_CREATE | O_RDWR);
      if (fd < 0)
	printf(1, "create failed\n");

      close(fd);
    }

    for (uint i = 0; i < nlookup; i++) {
      snprintf(pn, sizeof(pn), "%s/f:%d:%d", dirs[tid], tid, (i % nfile));
      int fd = open(pn, O_RDWR);
      if (fd < 0)
	printf(1, "open failed\n");

      close(fd);
    }

    for (uint i = 0; i < nfile; i++) {
      snprintf(pn, sizeof(pn), "%s/f:%d:%d", dirs[tid], tid, i);
      if (unlink(pn) < 0)
	printf(1, "unlink failed\n");
    }
  }

  exit();
}

int
main(void)
{
  for (uint i = 0; i < nthread; i++) {
    //snprintf(dirs[i], sizeof(dirs[i]), "/db%d", i);
    snprintf(dirs[i], sizeof(dirs[i]), "/dbx");
    if (mkdir(dirs[i]) < 0)
      printf(1, "mkdir failed\n");
  }

  mtrace_enable_set(1, "xv6-dirbench");

  printf(1, "dirbench[%d]: start\n", getpid());
  for(uint i = 0; i < nthread; i++) {
    int pid = fork(0);
    if (pid == 0)
      bench(i);
  }

  for (uint i = 0; i < nthread; i++)
    wait();

  mtrace_enable_set(0, "xv6-dirbench");
  printf(1, "dirbench[%d]: done\n", getpid());
  halt();
  exit();
}
