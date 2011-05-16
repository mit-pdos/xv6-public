#include "types.h"
#include "stat.h"
#include "user.h"
#include "xv6-mtrace.h"
#include "x86.h"
#include "uspinlock.h"

int
main(void)
{
  volatile char *p = (char *) 0x80000;
  if (map((void *) p, 8192) < 0) {
    printf(1, "map failed\n");
    exit();
  }

  p[0] = 'x';
  p[4096] = 'y';
  if (p[0] != 'x' || p[4096] != 'y') {
    printf(1, "mismatch\n");
    exit();
  }

  if (unmap((void *) p, 8192) < 0) {
    printf(1, "unmap failed\n");
    exit();
  }

  p[0] = 'z';
  if (p[0] == 'z') {
    printf(1, "still mapped\n");
    exit();
  }

  exit();
}
