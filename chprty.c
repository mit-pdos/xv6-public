// #include "proc.h"
// #include "defs.h"
// #include "ulib.h"
#include "user.h"

int main(int argc , char* argv[])
{
  if(argc!=3)
  {
    printf(1,"Usage: ps <pid> <priority no. (between 0 and 100)>\n");
  }

  else{
    int pid = atoi(argv[1]);
    if(pid == 1 || pid == 2)
    {
      printf(1,"Access Denied\n");
      exit();
    }
    int priority = atoi(argv[2]);
    if(priority < 0 || priority > 100){
      printf(1,"Priority should range between 0 and 100\n");
      exit();
    }
    chprty(pid, priority);

  }
  exit();
  return 0;
}
