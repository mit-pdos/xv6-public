#include "types.h"
#include "stat.h"
#include "user.h"
#include "proc_info.h"

//print process info with formatting
void print_proc(struct proc_info *info) {
  printf(0, "PID: %d | NAME: %s | MEMSIZE: %d\n" , info -> pid, info -> name, info -> memsize);
}

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

    printf(0, "\n\nPID NAME        MEMSIZE\n"); // fancy output header! *__* 

    for (int i = 0; i < 100; i++)
    {
      if (p[i].pid == 0) break; //A zero pid indicates the end of process list
      print_proc(&p[i]);
    }
  } else
  {
    printf(0, "Error forking");
  }
  exit();
}
