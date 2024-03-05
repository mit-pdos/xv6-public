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

int sys_ticks_running(void) {
  int pid;

  if(argint(0,&pid) < 0) return -1;
  return ticks_running(pid);
}

int sys_set_lottery_tickets(void) {
  int tickets;
  if(argint(0, &tickets) < 0) return -1;
  myproc()->lottery_tickets = tickets;
  return 0;
}
int sys_get_lottery_tickets(void) {
  int pid;
  if (argint(0, &pid) < 0) return -1;
  struct proc *p = get_proc(pid);
  if (p == 0 || p->state == UNUSED || p->state == ZOMBIE) return -1;
  return p->lottery_tickets;
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
sys_uniq(void)
{
  char *filename;
  int flags;
  int n;


  if(argstr(0, &filename) < 0)
    return -1;
  
  if(argint(1, &flags) < 0)
    return -1;

  if(argint(2, &n) < 0)
    return -1;

  return 0;
}
