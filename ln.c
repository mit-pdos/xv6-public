#include "types.h"
#include "user.h"

int
main(int argc, char *argv[])
{
  int (*ln)(char*, char*);
  
  ln = link;
  if(argc > 1 && strcmp(argv[1], "-s") == 0){
    ln = symlink;
    argc--;
    argv++;
  }
  
  if(argc != 3){
    printf(2, "usage: ln [-s] old new (%d)\n", argc);
    exit();
  }
  if(ln(argv[1], argv[2]) < 0){
    printf(2, "%s failed\n", ln == symlink ? "symlink" : "link");
    exit();
  }
  exit();
}
