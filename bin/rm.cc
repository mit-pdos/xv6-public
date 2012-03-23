#include "types.h"
#include "stat.h"
#include "user.h"

int
main(int argc, char *argv[])
{
  int i;

  if(argc < 2)
    die("Usage: rm files...");

  for(i = 1; i < argc; i++){
    if(unlink(argv[i]) < 0)
      die("rm: %s failed to delete\n", argv[i]);
  }

  exit();
}
