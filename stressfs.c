// Demonstrate that moving the "acquire" in iderw after the loop that
// appends to the idequeue results in a race.

// For this to work, you should also add a spin within iderw's
// idequeue traversal loop.  Spinning 40000 times demonstrated the bug
// after about 5 runs of stressfs in QEMU on a 2.1GHz CPU.

#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "fcntl.h"

int
main(int argc, char *argv[])
{
  int i;
  printf(1, "stressfs starting\n");

  for(i = 0; i < 4; i++){
    if(fork() > 0){
      break;
    }
  }

  printf(1, "%d\n", i);

  char path[] = "stressfs0";
  path[8] += i;
  int fd = open(path, O_CREATE | O_RDWR);
  for(i = 0; i < 100; i++)
    printf(fd, "%d\n", i);
  close(fd);

  wait();
  
  exit();
}
