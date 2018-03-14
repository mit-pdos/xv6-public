#include "types.h"
#include "stat.h"  //No se si es necesario
#include "user.h"

int
main(int argc, char *argv[])
{
  printf(1, "%d \n" , getprocs());
  exit();
}
