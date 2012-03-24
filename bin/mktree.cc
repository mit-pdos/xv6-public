#include "types.h"
#include "stat.h"
#include "user.h"
#include "lib.h"
#include "fcntl.h"

static void
dolevel(int fd, int branch, int depth)
{
  if (depth > 0) {
    for (int i = 0; i < branch; i++) {
      char name[] = "a";
      *name += i;
      if (mkdirat(fd, name) < 0)
        die("mkdirat");

      int nfd = openat(fd, name, O_RDONLY);
      if (nfd < 0)
        die("openat");
      dolevel(nfd, branch, depth-1);
    }
  }

  close(fd);
}

int
main(int ac, char **av)
{
  if (ac < 4)
    die("usage: %s dir branch depth", av[0]);

  const char *dir = av[1];
  int branch = atoi(av[2]);
  int depth = atoi(av[3]);

  if (mkdir(dir))
    die("mkdir");
  
  int fd = open(dir, O_RDONLY);
  if (fd < 0)
    die("open");
  
  dolevel(fd, branch, depth);
}
