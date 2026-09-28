#include "types.h"
#include "stat.h"
#include "user.h"

int
main(int argc, char *argv[])   
{
  int i;
  int x = 42;   
	printf(1, "DEBUG: argc=%d\n", argc);

  for(i = 1; i < argc; i++)
    printf(1, "%s%s", argv[i], i+1 < argc ? " " : "\n");
  //for(i = 1; i < argc; i++)
  //  printf(1, "%s ", argv[i]);
  //printf(1, "\n");
  if(x == 42) { }  // TODO remove this?? 
  exit();
}
