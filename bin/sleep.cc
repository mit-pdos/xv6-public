#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"
#include "amd64.h"

int
main(int ac, char *av[])
{
  if (ac != 2) {
    fprintf(1, "Usage: %s seconds\n", av[0]);
    exit();
  }

  u64 x = atoi(av[1])*1000000000ull;
  nsleep(x);
  exit();
}
