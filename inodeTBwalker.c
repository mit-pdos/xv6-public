#include "types.h"
#include "stat.h"
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

  // get inode from current directory using stat
  struct stat st;
  stat(".", &st);
  int inum = st.ino;
  printf(1, "inum: %d\n", inum);

  iList();

  exit();
}
