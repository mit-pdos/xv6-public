// Tests to drive abstract sharing analysis

#include "types.h"
#include "user.h"
#include "fcntl.h"
#include "mtrace.h"

static int cpu;

void
next()
{
  if (setaffinity(cpu) < 0) {
    cpu = 0;
    if (setaffinity(cpu) < 0)
      die("sys_setaffinity(%d) failed", cpu);
  }
  cpu++;
}

void
vmsharing(void)
{
  for (int i = 0; i < 8; i++) {
    next();

    volatile char *p = (char*)(0x40000UL + i * 4096);
    mtenable("xv6-mapsharing");
    if (map((void *) p, 4096) < 0)
      die("map failed");
    mtdisable("xv6-mapsharing");

    mtenable("xv6-mapsharing");
    if (unmap((void *) p, 4096) < 0)
      die("unmap failed");
    mtdisable("xv6-mapsharing");
  }
}

void
fssharing(void)
{
  // Note that we keep these files open; otherwise all of these
  // operations will share the abstract FD object and we won't get any
  // results.

  next();
  mtenable("xv6-fssharing");
  open("a", O_CREATE|O_RDWR);
  mtdisable("xv6-fssharing");

  next();
  mtenable("xv6-fssharing");
  open("b", O_CREATE|O_RDWR);
  mtdisable("xv6-fssharing");

  next();
  mtenable("xv6-fssharing");
  open("a", O_RDWR);
  mtdisable("xv6-fssharing");

  next();
  mtenable("xv6-fssharing");
  open("b", O_RDWR);
  mtdisable("xv6-fssharing");
}

int
main(int ac, char **av)
{
  if (ac == 2 && strcmp(av[1], "vm") == 0)
    vmsharing();
  else if (ac == 2 && strcmp(av[1], "fs") == 0)
    fssharing();
  else
    fprintf(1, "usage: %s vm|fs\n", av[0]);
}
