#include "user.h"

int
main(int argc, char *argv[])
{
  int i;

  for(i = 0; i < argc; i++){
    puts(argv[i]);
    puts(" ");
  }
  puts("\n");
  exit();
}
