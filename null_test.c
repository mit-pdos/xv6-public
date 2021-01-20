#include "types.h"
#include "stat.h"
#include "user.h"

int
main(int argc, char *argv[])
{
   int ppid = getpid();

   if (fork() == 0) {
      uint * nullp = (uint*)0;
      printf(1, "null dereference: ");
      printf(1, "%x %x\n", nullp, *nullp);
      // this process should be killed
      printf(1, "TEST FAILED\n");
      kill(ppid);
      exit();
   } else {
      wait();
   }

   printf(1, "TEST PASSED\n");
   exit();
}
