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

int
sys_brk_de(int input_addr)
{
  int new_addr;
  int old_addr;
  int n;
  uint sz;

  // input address location, then change to int value
  if (argint(0, &input_addr) < 0)
    return -1;
  old_addr = myproc()->sz;
  cprintf("proc old address: %p\n", old_addr);
  n = input_addr - myproc()->sz;

  // check the sign of offset, if negative, free memory, otherwise move the physical memory address
  if (n < 0) {
    new_addr = myproc()->sz - n;
    if ((sz = deallocuvm(myproc()->pgdir, old_addr, new_addr)) == 0) {
      cprintf("Deallocating failed...\n");
      return -1;
    } else {
      myproc()->sz -= n;
      cprintf("proc new address: %p\n", myproc()->sz);
    }
  } else {
    new_addr = myproc()->sz + n;
    if ((sz = allocuvm(myproc()->pgdir, old_addr, new_addr)) == 0) {
      cprintf("Allocating failed...\n");
      return -1;
    } else {
      myproc()->sz += n;
      cprintf("proc new address: %p\n", myproc()->sz);
    }
  }
  // move virtual memory address
  if (growproc(n) < 0)
    return -1;
  new_addr = myproc()->sz;
  return new_addr;
}

int
sys_sbrk_de(int nbyte)
{
  int old_addr;
  int new_addr;
  int n;
  uint sz;
  // input offset amount, then change to int value
  n = nbyte;
  if (argint(0, &n) < 0)
    return -1;

  old_addr = myproc()->sz;
  cprintf("proc old address: %p\n", old_addr);

  // check the sign of offset, if negative, free memory, otherwise move the physical memory address
  if (n < 0) {
    new_addr = myproc()->sz - n;
    if ((sz = deallocuvm(myproc()->pgdir, old_addr, new_addr)) == 0) {
      cprintf("Deallocating failed...\n");
      return -1;
    } else {
      myproc()->sz -= n;
      cprintf("proc new address: %p\n", myproc()->sz);
    }
  } else {
    new_addr = myproc()->sz + n;
    if ((sz = allocuvm(myproc()->pgdir, old_addr, new_addr)) == 0) {
      cprintf("Allocating failed...\n");
      return -1;
    } else {
      myproc()->sz += n;
      cprintf("proc new address: %p\n", myproc()->sz);
    }
  }
  // move virtual memory address
  if (growproc(n) < 0)
    return -1;
  new_addr = myproc()->sz;
  return new_addr;
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

//sys_calloc

int
sys_calloc(void)
{
  return calloc();
}

//sys_csbrk

int
sys_csbrk(void)
{
  int addr;
  int n;

  if (argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if (cgrowproc(n) < 0)
    return -1;
  return addr;
}

int
sys_realloc(void)
{
  printf(1,"[success] sysproc.c sys_realloc(void) test!\n");
  return realloc();
}
