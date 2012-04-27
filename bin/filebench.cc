#if defined(LINUX)
#include "user/util.h"
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#define O_CREATE O_CREAT
#define xfork() fork()
#define xexit() exit(EXIT_SUCCESS)
#else
#include "types.h"
#include "user.h"
#include "fcntl.h"
#define xfork() fork(0)
#define xexit() exit()
#endif

#define CHUNKSZ 512
#define FILESZ  (NCPU*CHUNKSZ)

static const bool pinit = true;

static char chunkbuf[CHUNKSZ];

static void
bench(int tid, int nloop)
{
  if (pinit)
    setaffinity(tid);

  int fd = open("filebenchx", O_RDONLY);
  if (fd < 0)
    die("open");

  for (int i = 0; i < nloop; i++) {
    ssize_t r;

    r = pread(fd, chunkbuf, CHUNKSZ, CHUNKSZ*tid);
    if (r != CHUNKSZ)
      die("pread");
  }

  close(fd);
  xexit();
}

int
main(int ac, char **av)
{
  int nthread;
  int nloop;

#ifdef HW_qemu
  nloop = 50;
#else
  nloop = 1000;
#endif

  if (ac < 2)
    die("usage: %s nthreads [nloop]", av[0]);

  nthread = atoi(av[1]);
  if (ac > 2)
    nloop = atoi(av[2]);

  // Setup shared file
  int fd = open("filebenchx", O_CREATE|O_WRONLY);
  if (fd < 0)
    die("open O_CREAT failed");
  for (int i = 0; i < FILESZ; i += CHUNKSZ) {
    int r = write(fd, chunkbuf, CHUNKSZ); 
    if (r < CHUNKSZ)
      die("write");
  }
  close(fd);

  for (int i = 0; i < nthread; i++) {
    int pid = xfork();
    if (pid == 0)
      bench(i, nloop);
    else if (pid < 0)
      die("fork");
  }

  return 0;
}
