#include "types.h"
#include "stat.h"
#include "user.h"
#include "proc_info.h"

//print process info with formatting
void print_proc(struct proc_info *info) {
  printf(0, "\nPID: %d | NAME: %s | MEMSIZE: %d\n" , info -> pid, info -> name, info -> memsize);
}

void print_processes(struct proc_info *p) 
{
    printf(0, "\nList of processes that are RUNNING or RUNNABLE sorted by memsize:");
    for (int i = 0; i < 100; i++)
    {
      if (p[i].pid == 0) break; //A zero pid indicates the end of process list
      print_proc(&p[i]);
    }
}

int test_memory_size[100] = {1000, 434, 245, 700, 100, 2000, 3000, 150, 400, 500, 140, 350, 600, 800};

/**
 * A recursive procedure used to create the test processes.
 */
void create_test_processes(int pcnt, int idx)
{
  malloc(idx * 10 * test_memory_size[idx] * sizeof(char *)); //Note: memory allocation is accumulative (so we are adding more memory to child's user space)

  if (idx == pcnt) while(1); //NOP

  int pid = fork();

  if (pid == 0) create_test_processes(pcnt, idx + 1);
  else if (pid > 0) while(1); //NOP
  else printf(0, "E: fork error");
}

int main(void)
{
  struct proc_info p[100];
  
  int pid = fork();

  if (pid > 0)
  {
    sleep(500); // Wait until some processes are created for testing
    rnps(p);
    print_processes(p);
  } else if (pid == 0)
  {
    create_test_processes(10, 0);
  } else
  {
    printf(0, "E: fork error in main");
  }

  exit();
}

