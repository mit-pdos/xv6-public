#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"
    
 //passing command line arguments 
    
int main(int argc, char *argv[]) 
{ 
  int f = open("./test.txt", O_RDWR);

  char b[10];
  read(f, b, 4);
  printf(1, "d1 : %s\n", b);
  printf(1, "%d\n", lseek(f, 10, 1));
  read(f, b, 5);
  printf(1, "d1 : %s\n", b);
  exit(); 
}
