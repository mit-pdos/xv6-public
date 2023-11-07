#include "types.h"
#include "stat.h"
#include "user.h"

static struct procInfo arrayOfProcInfo[1024];
static const char * const states[] = {
  [UNUSED]    "unused",
  [EMBRYO]    "embryo",
  [SLEEPING]  "sleep ",
  [RUNNABLE]  "ready ",
  [RUNNING]   "run   ",
  [ZOMBIE]    "zombie"
  };

int
main(int argc, char *argv[])
{
  printf(1, "priority of PID 1 is %d\n", nice(1, 0));
  int result = ps(1024, arrayOfProcInfo);
  for(int i = 0; i < result; ++i) {
    printf(1, "%d: %s %d%s %s prio: %d\n", arrayOfProcInfo[i].pid,
      states[arrayOfProcInfo[i].state], 
      arrayOfProcInfo[i].cpuPercent, "%",
      arrayOfProcInfo[i].name, arrayOfProcInfo[i].priority);
  }

  exit();
}
