#include "types.h"
#include "stat.h"
#include "fcntl.h"
#include "user.h"
#include "amd64.h"
#include "ipc.h"

// XXX(sbw) add a memlayout.h?
#define KSHARED 0xFFFFF00000000000ull

#define FSIZE (64 << 10)
#define BSIZE 4096
#define PSIZE (4*BSIZE)

static char buf[BSIZE];

struct ipcctl *ipcctl = (struct ipcctl*)KSHARED;

static void
kernlet_pread(int fd, size_t count, off_t off)
{
  ipcctl->done = 0;
  if (kernlet(fd, count, off) != 0)
    die("kernlet");
}

int
main(int ac, char **av)
{
  u64 t0, t1;
  int i, k;
  int fd;
  
  fd = open("preadtest.x", O_CREATE|O_RDWR);
  if (fd < 0)
    die("open failed");
  
  for (i = 0; i < FSIZE/BSIZE; i++)
    if (write(fd, buf, BSIZE) != BSIZE)
      die("write failed");

  t0 = rdtsc();
  for (k = 0; k < FSIZE; k+=PSIZE) {
    kernlet_pread(fd, PSIZE, k);

    for (i = k; i < k+PSIZE; i+=BSIZE)
      if (pread(fd, buf, BSIZE, i) != BSIZE)
        die("pread failed");
  }
  t1 = rdtsc();
  
  printf(1, "cycles %lu\n", t1 - t0);
  
  exit();
}
