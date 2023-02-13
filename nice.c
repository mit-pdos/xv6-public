#include "types.h"
#include "stat.h"
#include "user.h"

int
main(int argc, char **argv)
{
  if(argc < 3){
    printf(2, "usage: nice pid priority...\n");
    exit();
  }
  nice(atoi(argv[1]), atoi(argv[2]));
  exit();
}
