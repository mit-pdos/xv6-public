#include "types.h"
#include "stat.h"
#include "user.h"
#include <unistd.h>
#include <stdlib.h>
 
int main(void) {
  char buf[100];

  printf(1, "West Chester University was founded in %d\n", wcupa());
  printf("read() count before: %d\n", getreadcount());
  int rt = read(0,buf,sizeof(buf));
  if (rt < 0)
	  exit(1);
  else
	  printf("read() count after: %d\n", getreadcount());
  exit(0);
}
