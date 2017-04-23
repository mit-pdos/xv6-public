#include "types.h"
#include "user.h"

#define PAGESIZE 4096
#define NUMPAGES 27

// @TODO: Make this actually do something
int
main(int argc, char *argv[])
{
  char *memory = sbrk(PAGESIZE*NUMPAGES);
  int i;
  
  for(i = 0; i < NUMPAGES; i++){
    memory[i*PAGESIZE] = i;
    
    if(SELECTION == NFU)
      memory[0] = 0;
  }

  for(i = 0; i < NUMPAGES; i++){
    if(memory[i*PAGESIZE] != i){
      printf(1, "incorrect data. page:%d, data:%d\n", i, memory[i*PAGESIZE]);
      break;
    }
  }
  
  exit();
}
