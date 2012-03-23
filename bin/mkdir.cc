#include "types.h"
#include "stat.h"
#include "user.h"

int
main(int argc, char *argv[])
{
  int i;

  if (argc < 2)
    die("ussage: mkdir files...");

  for(i = 1; i < argc; i++) {
    if (mkdir(argv[i]) < 0)
      die("mkdir: %s failed to create", argv[i]);
  }
}
