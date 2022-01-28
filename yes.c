#include "types.h"
#include "stat.h"
#include "user.h"

int
main(int argc, char *argv[])
{
  for (int i = 0; i < 100; i++)
    	printf(1, "%s\n", (argc>1? argv[1]: "y"));
      // printf(1, "y\n");
  exit();
}

