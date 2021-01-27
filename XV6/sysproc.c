#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "pstat.h"


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

//return number of system calls called till now
int 
sys_count(void){
    return count();
}

// -----------------------------------------OUR WORK------------------------------------------

int
sys_settickets(void)
{
  int num_tickets;
  if (argint(0, &num_tickets) < 0)
    return -1;
  
  settickets(num_tickets);
  return 0;
}

int
sys_getpinfo(void)
{
  struct pstat *p;
  if (argptr(0, (char **)&p, sizeof(struct pstat)) < 0)
    return -1;

  getpinfo(p);
  return 0;
}

//set addr to read
int sys_mprotect(void){
  void *addr;
  int len;
  //return -1 when
  //addr is not page alligned or 
  //addr points to a part of the address space or
  //len <= 0 
  if(argptr(0,(void*)&addr,sizeof(void*))<0||argint(1,&len)<0){
    cprintf("\nzero length!\n");
    return -1; 
  } 
  if(len <= 0){
    cprintf("\nzero/negative length!\n");
    return -1;
  }
  if((int)(((int)addr)%PGSIZE)){
    cprintf("\nnot page aligned!\n");
    return -1;
  }
  return mprotect(addr,len);
}

//set addr to read/write
int sys_munprotect(void){
  void * addr;
  int len;
  //return -1 when
  //addr is not page alligned or 
  //addr points to a part of the address space or
  //len <= 0 
  if(argptr(0,(void*)&addr,sizeof(void*))<0||argint(1,&len)<0){
    cprintf("\nzero length!\n");
    return -1; 
  }
  if(len <= 0){
    cprintf("\nzero/negative length!\n");
    return -1;
  }
  if((int)(((int)addr)%PGSIZE)){
    cprintf("\nnot page aligned!\n");
    return -1;
  }
  return munprotect(addr,len);
}

//threads
int
sys_clone(void){

  int fcn , arg1 , arg2 ,stack;
  if(argint(0,&fcn)<0)return -1;
  if(argint(1,&arg1)<0)return -1;
  if(argint(2,&arg2)<0)return -1;
  if(argint(3,&stack)<0)return -1; 

  return clone((void *)fcn, (void *)arg1, (void *)arg2,(void *)stack);
}

int 
sys_join(){

  void **stack;
  int stackArg;
  stackArg = argint(0, &stackArg);
  stack = (void**) stackArg;

  return join(stack);
}


