#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h" // for O_CREATE

int
main(int argc, char *argv[])
{
  int lockFile = open("lock", O_CREATE | O_RDWR);
  flock(lockFile);

  int highPID = fork();
  if(0 == highPID) {
    // In child
    int pid = getpid();
    for(int i = 0; i < (1 << 30); ++i) {

    }
    nice(pid, 4);
    printf(2, "Started High PID: %d priority: %d\n", pid, 4);
    while(1) {
      flock(lockFile);
      printf(2, "In High PID: %d priority: %d\n", pid, 4);
      funlock(lockFile);
      exit();
    }
  } else {
    // In parent
    int pid = getpid();
    nice(getpid(), 0);
    printf(2, "Started Low PID: %d priority: %d\n", pid, 0);
    for(int i = 0; i < (1 << 30); ++i) {

    }

    if(1 < argc) {
      // Create medium priority process that prevents low priority
      // process from running
      int mediumPID = fork();
      if(0 == mediumPID) {
        int pid = getpid();
        nice(pid, 2);
        printf(2, "Started Med PID: %d priority: %d\n", pid, 2);
        while(1) {
          for(int i = 0; i < (1 << 30); ++i) {

          }
          printf(2, "In Med PID: %d priority: %d\n", pid, 2);
        }
      } else {
        int pid = getpid();
        while(1) {
          for(int i = 0; i < (1 << 30); ++i) {

          }
          printf(2, "In Low PID: %d priority: %d\n", pid, 0);
          funlock(lockFile);
        }
      }
    } else {
      // There is no Medium priority process
      int pid = getpid();
      while(1) {
        for(int i = 0; i < (1 << 30); ++i) {

        }
        printf(2, "In Low PID: %d priority: %d\n", pid, 0);
        funlock(lockFile);
      }
    }
  }

  exit();
}
