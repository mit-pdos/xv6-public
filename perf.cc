#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"
#include "sampler.h"

static u64 selector = 
  0UL << 32 |
  1 << 24 | 
  1 << 22 | 
  1 << 20 |
  1 << 17 | 
  1 << 16 | 
  0x00 << 8 | 
  0x76;
static u64 period = 100000;

static void
conf(int fd, sampop_t op)
{
  struct sampconf c = { 
    op: op,
    selector: selector,
    period: period,
  };

  if (write(fd, &c, sizeof(c)) != sizeof(c))
    die("perf: write failed");
  close(fd);
}

int
main(int ac, const char *av[])
{
  int fd = open("/dev/sampler", O_RDWR);
  if (fd < 0)
    die("perf: open failed");

  int pid = fork(0);
  if (pid < 0)
    die("perf: fork failed");

  if (pid == 0) {
    conf(fd, SAMP_ENABLE);
    exec(av[1], av+1);
    die("perf: exec failed");
  }
  
  wait();
  conf(fd, SAMP_DISABLE);
  exit();
}
