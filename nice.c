#include "types.h"
#include "stat.h"
#include "user.h"


int
main(int argc, char *argv[])
{
  if(argc < 3 || 3 < argc) {
    printf(2, "Usage: %s pid priority\n", argv[0]);
  } else {
    int pid = atoi(argv[1]);
    int priority = atoi(argv[2]);
    int oldPriority = nice(pid, priority);
    printf(2, "Changing PID: %d to %d\n", pid, priority);
    printf(1, "%d\n", oldPriority);
  }

  exit();
}
