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

  if (argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return myproc()->pid;
}

// ===== brk and sbrk related sys call =====
// int
// sys_sbrk_final(void)
// {
//   int addr;
//   int n;
//   uint sz;

//   if(argint(0, &n) < 0)
//     return -1;
//   addr = myproc()->sz;
//   myproc()->sz += n;
//   if(n < 0){
//     if((sz = deallocuvm(myproc()->pgdir, addr, myproc()->sz)) == 0){
//       cprintf("Deallocating failed...\n");
//       return -1;
//     }
//   }
//   if(growproc(n) < 0)
//     return -1;
//   return addr;
// }

int
sys_sbrk_de(int nbyte)
{
  int addr;
  int n;
  uint sz;
  // input offset amount
  n = nbyte;

  // whether offset viable
  if (argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  cprintf("proc old address: %p\n", addr);
  // check the sign of offset, if negative, free memory, otherwise move the physical memory address
  if (n < 0) {
    if ((sz = deallocuvm(myproc()->pgdir, addr, myproc()->sz)) == 0) {
      cprintf("Deallocating failed...\n");
      return -1;
    } else {
      myproc()->sz -= n;
      cprintf("proc new address: %p\n", myproc()->sz);
    }
  } else {
    myproc()->sz += n;
    cprintf("proc new address: %p\n", myproc()->sz);
  }
  // move virtual memory address
  if (growproc(n) < 0)
    return -1;
  addr = myproc()->sz;
  return addr;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if (argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if (growproc(n) < 0)
    return -1;
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;

  if (argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while (ticks - ticks0 < n) {
    if (myproc()->killed) {
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
sys_cps(void)
{
  return cps();
}

int
sys_calloc(void)
{
  return calloc();
}