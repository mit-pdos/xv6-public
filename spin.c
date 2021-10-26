#include "types.h"
#include "stat.h"
#include "user.h"
#include "pstat.h"


int
main(int argc, char *argv[])
{
   // struct pstat st;

   // check(getpinfo(&st) == 0, "getpinfo");
   // check(getpinfo(NULL) == -1, "getpinfo with bad pointer");
   // check(getpinfo((struct pstat *)1000000) == -1, "getpinfo with bad pointer"); 
   printf(1, "Get p info running %d\n", getpinfo());
   printf(1, "Settickets is workings %d\n", settickets(5));


   exit();
}