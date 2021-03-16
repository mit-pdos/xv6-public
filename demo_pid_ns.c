#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"
#include "ns_types.h"
#include "param.h"


void
panic(char *s)
{
  printf(stderr, "%s\n", s);
  exit(1);
}

int
main(int argc, char *argv[])
{

      if(unshare(PID_NS) != 0){
        printf(stderr, "Cannot create pid namespace\n");
        return -1;
      }

      int pid = fork();
      if(pid == -1){
         panic("fork");
      }

      if(pid == 0)
        printf(1, "New namespace. PID=%d\n", getpid());
      else
        printf(1, "Parent's perspective on the child. PID=%d\n", pid);

      exit(0);
}
