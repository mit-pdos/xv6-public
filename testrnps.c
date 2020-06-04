#include "types.h"
#include "stat.h"
#include "user.h"
#include "proc_info.h"

int main(void)
{
  struct proc_info p[100];
  
  int pid = fork();

  if (pid > 0)
  {
    wait();
    printf(0, "Continuing parent");
  } else if (pid == 0)
  {
    printf(0, "In Child now");
    rnps(p);
    for (int i = 0; i < 100; i++)
    {
      if (p[i].pid == 0) break; //A zero pid indicates the end of process list
      printf(0, "\nprocess => pid : %d, name : %s\n", p[i].pid, p[i].name);
    }
  } else
  {
    printf(0, "Error forking");
  }
  exit();
}
