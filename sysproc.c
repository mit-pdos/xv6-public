#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "steady_clock.h"
#include "ioctl_request.h"

int
sys_fork(void)
{
  return fork();
}

int
sys_exit()
{
  int status;

  if (argint(0, &status) < 0)
    return -1;
  exit(status);
  return 0;  // not reached
}

int
sys_wait(void)
{
  int* wstatus;

  if (argptr(0, (void*)&wstatus, sizeof(*wstatus)) < 0)
    return -1;
  return wait(wstatus);
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
  return myproc()->ns_pid;
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

int
sys_usleep(void)
{
  int n;

  if(argint(0, &n) < 0)
    return -1;
  unsigned int start = steady_clock_now();
  acquire(&tickslock);
  while(steady_clock_now() - start < (unsigned int)n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

int
sys_ioctl(void)
{
  int fd = -1;
  int request = -1;

  if (argint(0, &fd) < 0) {
    return -1;
  }

  if (argint(1, &request) < 0) {
    return -1;
  }

  int result;
  switch (request) {
  case IOCTL_GET_PROCESS_CPU_PERCENT:
    proc_lock();
    result = myproc()->cpu_percent;
    proc_unlock();
    return result;
  case IOCTL_GET_PROCESS_CPU_TIME:
    proc_lock();
    result = myproc()->cpu_time;
    proc_unlock();
    return result;
  default:
    return -1;
  }
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
