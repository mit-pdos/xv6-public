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
  int result = ps(1024, arrayOfProcInfo);
  for(int i = 0; i < result; ++i) {
    printf(1, "%d: %s %d %s\n", arrayOfProcInfo[i].pid,
      states[arrayOfProcInfo[i].state], 
      arrayOfProcInfo[i].cpuPercent,
      arrayOfProcInfo[i].name);
  }
  exit();
}
