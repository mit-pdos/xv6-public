#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"

int sys_clone(void)
{

    void (*func)(void *,void *);
    char *child_stack, *arg1,*arg2;

    if(argptr(0, (char **)&func, 0) == -1) {
        return -1;
    }
    
    if(argptr(1, &arg1, 0) == -1) {
        return -1;
    }
    
    if(argptr(2, &arg2, 0) == -1) {
        return -1;
    }
    if(argint(3, (int *)&child_stack) == -1) {
        return -1;
    }
    
    
    if(myproc()->sz < (uint)arg1 + (uint)arg2) {
        return -1;
    }

    
    if(myproc()->sz <= (uint)child_stack && 
        myproc()->sz < (uint)child_stack - PGSIZE) {
        return -1;
    }
    
    
  return clone(func,(void*)arg1,(void*)arg2,(void *)child_stack);

}
int sys_join(void)
{
  int Thread_id;
  if(argint(0,&Thread_id)<0)
    return -1;
  return join(Thread_id);  
}
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
