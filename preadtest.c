#include "types.h"
#include "stat.h"
#include "fcntl.h"
#include "user.h"
#include "amd64.h"

#define FSIZE (64 << 10)
#define BSIZE 4096

static char buf[BSIZE];

int
main(int ac, char **av)
{
  u64 t0, t1;
  int fd;
  int i;
  
  fd = open("preadtest.x", O_CREATE|O_RDWR);
  if (fd < 0)
    die("open failed");
  
  for (i = 0; i < FSIZE/BSIZE; i++)
    if (write(fd, buf, BSIZE) != BSIZE)
      die("write failed");

  t0 = rdtsc();
  for (i = 0; i < FSIZE; i+=BSIZE)
    if (pread(fd, buf, BSIZE, i) != BSIZE)
      die("pread failed");
  t1 = rdtsc();
  
  printf(1, "cycles %lu\n", t1 - t0);
  
  exit();
}
