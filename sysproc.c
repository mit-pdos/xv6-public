#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "proc_info.h"

int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return myproc()->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

int
sys_rnps(void)
{
  struct proc_info *p_infos; //array of process info structs (could also use array of pointers)

  if (argptr(0, (void*)&p_infos, sizeof(*p_infos)) < -1)
    return -1;

  get_process_infos(p_infos);
  //cprintf("\n\nrnps system call, there was no issue getting the argument hello \n\n");
  return 0;
}

int
sys_waitx(void)
{
  int *rtime, *wtime;
  if (argptr(0, (char**)&rtime, sizeof(int)) < 0 || argptr(1, (char**)&wtime, sizeof(int) < 0))
  {
    return -1;
  }
  return waitx(wtime, rtime);
}

int
sys_spri(void)
{
  int priority;
  if (argptr(0, (void*)&priority, sizeof(int)) < 0)
  {
    return -1;
  }
  return set_priority(priority); 
}
