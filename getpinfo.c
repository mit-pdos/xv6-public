#include "proc_stat.h"
// #include "defs.h"
#include "user.h"

int main(int argc , char* argv[])
{
  if(argc != 2){
    printf(1,"Usage: getpinfo <pid>");
    exit();
    return 0;
  }
  int r;
  int pid = atoi(argv[1]);
  struct pstat Pstat;
  if((r = getpinfo(pid, &Pstat)) < 0)
  {
    printf(1,"Error!\n");
    exit();
    return 0;
  }
  printf(1,"Run time of process with pid %d is %d\n",Pstat.pid,Pstat.run_time);
  exit();
  return 0;

}
