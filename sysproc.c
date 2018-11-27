#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "spinlock.h"
#include "uproc.h"

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
sys_backtrace(void)
{
  struct proc *p = myproc();
  cprintf("eax:0x%x\n", p->tf->eax);
  cprintf("ebx:0x%x\n", p->tf->ebx);
  cprintf("ecx:0x%x\n", p->tf->ecx);
  cprintf("edx:0x%x\n", p->tf->edx);
  cprintf("edi:0x%x\n", p->tf->edi);
  cprintf("esi:0x%x\n", p->tf->esi);

  cprintf("cs:0x%x\n", p->tf->cs);
  cprintf("ds:0x%x\n", p->tf->ds);
  cprintf("es:0x%x\n", p->tf->es);
  cprintf("fs:0x%x\n", p->tf->fs);
  cprintf("gs:0x%x\n", p->tf->gs);
  cprintf("ss:0x%x\n", p->tf->ss);

  cprintf("eflags:0x%x\n", p->tf->eflags);
  cprintf("err:0x%x\n", p->tf->err);
  
  cprintf("esp:0x%x\n", p->tf->esp);
  cprintf("eip:0x%x\n", p->tf->eip);
  cprintf("ebp:0x%x\n", p->tf->ebp);

  int count = 0;
  uint ebp = p->tf->ebp;
  uint return_addr = 0x0;
  while (return_addr != 0xffffffff) {    
    memmove(&return_addr, (const void *)(ebp + 4), sizeof(uint));
    cprintf("%d#  0x%x\n", count++, return_addr);
    memmove(&ebp, (const void *)ebp, sizeof(uint));
  } 

  return 0;
}

struct {
  struct spinlock lock;
  struct proc proc[NPROC];
} ptable;

int
sys_getprocinfo(void)
{
  int proc_num;
  char *tmp; 

  if(argint(0, &proc_num) < 0 || argptr(1, &tmp, sizeof(struct uproc)) < 0 )
    return -1;
  
  if(proc_num >= NPROC || proc_num < 0) {
    return -1;
  }

  struct uproc *up = (struct uproc *)tmp;
  struct proc *p = &ptable.proc[proc_num];

  if (p->state == UNUSED) {
    return 1;
  }

  memmove(up->name, p->name, 16);
  up->pid = p->pid;
  up->parentpid = up->pid == 1 ? 0 : p->parent->pid;  // The parent pid of first process init should be 0 (None)
  up->sz = p->sz;
  up->state = p->state;
  up->chan = p->chan == 0 ? 0 : 1;
  up->killed = p->killed == 0 ? 0 : 1;

  return 0;
}
