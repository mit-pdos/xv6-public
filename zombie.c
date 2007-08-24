// Create a zombie process.

#include "types.h"
#include "stat.h"
#include "user.h"

int
main(void)
{
  int i;

  if(fork() > 0)
    for(i=0; i<10; i++)
      yield();
  exit();
}
