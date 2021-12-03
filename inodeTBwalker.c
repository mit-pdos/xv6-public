#include "types.h"
#include "user.h"
#include "fs.h"


int
main(int argc, char *argv[])
{
  //int inum;
  if(argc > 1){
    printf(1, "no arguements supported");
    exit();
  }
  iList();
  exit();
}