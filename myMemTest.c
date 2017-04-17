#include "types.h"
#include "user.h"

int
main(int argc, char *argv[])
{
  //int pid = getpid();
  char* memory = (char*) sbrk(4096*30);
  
  //access first 15 pages of the memory chunk
  for (int i = 0; i < 15; i++)
  {
    memory[i*4096] = i;
  }
  
  switch (SELECTION)
  {
    case NFU:
    {
      //access pages 16,1,17,1,18,1,19,1... etc. page 1 shouldn't get replaced
      for (int i = 15; i < 30; i++)
      {
        memory[i*4096] = i;
        memory[0*4096] = 0;
      }
    }
    case FIFO:
    {
      //access pages 16-30 of the memory chunk. pages replaced should be 1,2,3,4,5... etc
      for (int i = 15; i < 30; i++)
      {
        memory[i*4096] = i;
      }
    }
  }
  
  //make sure data in pages is correct
  for (int i = 0; i < 30; i++)
  {
    if (!(memory[i*4096] == i))
    {
      printf(1, "incorrect data. page:%d, data:%d\n", i, memory[i*4096]);
      break;
    }
  }
  
  exit();
}