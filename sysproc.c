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

extern void *GetSharedPage(int key, int len);
void* sys_GetSharedPage(void)
{
  int key,len;

  if(argint(0, &key) < 0 || argint(1, &len) < 0)
    return (void*)-1;
  return (void*)(GetSharedPage(key, len));
}

extern int FreeSharedPage(int key);
int sys_FreeSharedPage(void)
{
  int key;
  if(argint(0, &key) < 0)
    return -1;
  return FreeSharedPage(key);
}
int 
//added 

sys_iErase(int inum)
{
  //call iErase
  ierase(inum);
  return 0;
}

//added 
int
sys_iList(void)
{
  ilist();
  return 0;
}

