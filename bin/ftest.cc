#include "types.h"
#include "stat.h"
#include "fcntl.h"
#include "user.h"
#include "lib.h"
#include "amd64.h"
#include "ipc.hh"
extern "C" {
#include "stream.h"
}

#define FSIZE (64 << 10)
#define BSIZE 4096
static char wbuf[512];
static char rbuf[BSIZE];

static int check = 0;

int
main(int ac, char **av)
{
  size_t count;
  off_t off;
  FILE *fp;
  int fd;
  int i;

  memset(ipcctl, 0, sizeof(*ipcctl));

  for (i = 0; i < sizeof(wbuf); i++)
    wbuf[i] = i % 16;
  
  unlink("ftest.x");
  fd = open("ftest.x", O_CREATE|O_RDWR);
  for (i = 0; i < FSIZE; ) {
    count = MIN(sizeof(wbuf), FSIZE-i);
    if (write(fd, wbuf, count) != count)
      die("write failed");
    i += count;
  }

  fp = fdopen(fd, "r");
  if (fp == 0)
    die("fdopen");

  off = 0;
  while ((count = fread(rbuf, 1, BSIZE, fp))) {
    if (check) {
      for (i = 0; i < count; i++)
        if (rbuf[i] != (i+off)%16)
          die("ftest %u: %u != %u", i, (int)(rbuf[i]), (off+i)%16);
      off += count;
    }
  }

  fclose(fp);
  exit();
}
