#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

int main(int argc, char *argv[])
{
  int pid1, pid2;
  int priority1, priority2;
  pid1 = 1;
  priority1 = 3;
  pid2 = 2;
  priority2 = 4;
  chpr(pid1, priority1);
  chpr(pid2, priority2);
  exit(0);
}