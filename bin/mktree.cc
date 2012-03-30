#include "types.h"
#include "stat.h"
#include "user.h"
#include "lib.h"
#include "fcntl.h"
#include "wq.hh"

static int branch;

static void
dolevel(int fd, int depth)
{
  if (depth > 0) {
    int it = 0;                                
    wq_for<int>(it,
                [](int &it)->bool { return it < branch; },
                [&fd, &depth](int i)->void
    {
      char name[] = "a";
      *name += i;
      if (mkdirat(fd, name) < 0)
        die("mkdirat");

      int nfd = openat(fd, name, O_RDONLY);
      if (nfd < 0)
        die("openat: %s at %u", name, depth);
      dolevel(nfd, depth-1);
    });
  }

  close(fd);
}

int
main(int ac, char **av)
{
  if (ac < 5)
    die("usage: %s nworkers dir branch depth", av[0]);

  wq_maxworkers = atoi(av[1])-1;
  initwq();

  const char *dir = av[2];
  branch = atoi(av[3]);
  int depth = atoi(av[4]);

  if (mkdir(dir))
    die("mkdir");
  
  int fd = open(dir, O_RDONLY);
  if (fd < 0)
    die("open");
  
  dolevel(fd, depth);
}
