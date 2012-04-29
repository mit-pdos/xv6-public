#if defined(LINUX)
#include "user/util.h"
#include "types.h"
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include <sys/stat.h>
#define O_CREATE O_CREAT
#define xfork() fork()
#define xexit() exit(EXIT_SUCCESS)
#define xmkdir(pathname) mkdir((pathname), S_IWUSR|S_IRUSR);
#define mtenable(x) do { } while(0)
#define mtdisable(x) do { } while(0)
#else
#include "types.h"
#include "stat.h"
#include "user.h"
#include "mtrace.h"
#include "amd64.h"
#include "fcntl.h"
#define xfork() fork(0)
#define xexit() exit()
#define xmkdir(pathname) mkdir((pathname))
#endif

static const bool pinit = true;

enum { nfile = 10 };
enum { nlookup = 100 };

void
bench(u32 tid, int nloop, const char* path)
{
  char pn[MAXNAME];

  if (pinit)
    setaffinity(tid);

  for (u32 x = 0; x < nloop; x++) {
    for (u32 i = 0; i < nfile; i++) {
      snprintf(pn, sizeof(pn), "%s/f:%d:%d", path, tid, i);

      int fd = open(pn, O_CREATE | O_RDWR);
      if (fd < 0)
	die("create failed\n");

      close(fd);
    }

    for (u32 i = 0; i < nlookup; i++) {
      snprintf(pn, sizeof(pn), "%s/f:%d:%d", path, tid, (i % nfile));
      int fd = open(pn, O_RDWR);
      if (fd < 0)
        die("open failed\n");

      close(fd);
    }

    for (u32 i = 0; i < nfile; i++) {
      snprintf(pn, sizeof(pn), "%s/f:%d:%d", path, tid, i);
      if (unlink(pn) < 0)
	die("unlink failed\n");
    }
  }

  xexit();
}

int
main(int ac, char** av)
{
  const char* path;
  int nthread;
  int nloop;
  
#ifdef HW_qemu
  nloop = 50;
#else
  nloop = 1000;
#endif
  path = "/dbx";

  if (ac < 2)
    die("usage: %s nthreads [nloop] [path]", av[0]);

  nthread = atoi(av[1]);
  if (ac > 2)
    nloop = atoi(av[2]);
  if (ac > 3)
    path = av[3];

  xmkdir(path);

  mtenable("xv6-dirbench");
  u64 t0 = rdtsc();
  for(u32 i = 0; i < nthread; i++) {
    int pid = xfork();
    if (pid == 0)
      bench(i, nloop, path);
    else if (pid < 0)
      die("fork");
  }

  for (u32 i = 0; i < nthread; i++)
    wait();
  u64 t1 = rdtsc();
  mtdisable("xv6-dirbench");

  printf("dirbench: %lu\n", t1-t0);
  return 0;
}
