#include "types.h"
#include "stat.h"
#include "user.h"

int main(void)
{
  int pid = fork();

  int wtime = 0;
  int rtime = 0;

  if (pid == 0)
  {
    printf(0, "Start sleep in child\n");
    sleep(400);
    printf(0, "Done sleep in child\n");
  } else if (pid > 0)
  {
    printf(0, "Start wait in parent\n");
    waitx(&wtime, &rtime);
    printf(0, "Done parent\n");
    printf(0, "wtime=%d, rtime=%d\n", wtime, rtime);
  } else
  {
    printf(0, "Error: fork error in main");
  }

  exit();
}

