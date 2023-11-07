#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"

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
sys_yield(void)
{
  yield();
  return 0;
}

int sys_shutdown(void)
{
  shutdown();
  return 0;
}

/// @brief This is the kernel side of the nice system call. 
/// arg0 is a valid pid, arg1 is an integer priority.
/// @param  
/// @return 
int sys_nice(void)
{
  int targetPID = -1;
  int targetPriority = 0;

  if(argint(0, &targetPID) < 0)
    return -1;
  if(argint(1, &targetPriority) < 0)
    return -1;

  return proc_nice(targetPID, targetPriority);
}

/// @brief This is the kernel side of a system call to obtain information 
/// about existing processes in the kernel
/// arg0 is the maximum number of elements storable in procInfoArray
/// arg1 is an array of struct procInfo able to store at least 
/// arg0 elements.
/// @return The number of struct procInfo structures stored in arg1.
/// This number may be less than arg0, and if it is, elements
/// at indexes >= arg0 may contain uninitialized memory.
int sys_ps(void)
{
  int numberOfProcs;
  struct procInfo* procInfoArray;

  if(argint(0, &numberOfProcs) < 0)
    return -1;
  if(argptr(1, (char **)&procInfoArray,  sizeof(struct procInfo *)) < 0)
    return -1;
  return proc_ps(numberOfProcs, procInfoArray);
}

/// @brief This function executes as result of system call
/// @return zero upon success and -1 otherwise
int sys_attachSharedMemory(void) 
{
  char **sharedUVM;
  if(argptr(0, (char **)&sharedUVM,  sizeof(char *)) < 0)
  {
    return -1;
  }

  return proc_attachSharedMemory(sharedUVM);
}
