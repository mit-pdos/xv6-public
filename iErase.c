#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"

int
main(int argc, char *argv[])
{
  int inum;
  if(argc < 2){
    printf(1, "Please Specify Inode");
    exit();
  }
  inum = atoi(argv[1]);
  if(iErase(inum) == 0){
    printf(1, "inode data cleared for %d", inum); 
    exit();
  }
  else{
    printf(1, "inode data not cleared for %d", inum); 
    exit();
  }

}
