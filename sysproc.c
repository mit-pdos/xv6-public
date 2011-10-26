#include "types.h"
#include "x86.h"
#include "kernel.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "spinlock.h"
#include "condvar.h"
#include "queue.h"
#include "proc.h"

#if 0
int
sys_fork(void)
{
  int flags;

  if(argint(0, &flags) < 0)
    return -1;
  return fork(flags);
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
  return proc->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = proc->brk;
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
    if(proc->killed){
      release(&tickslock);
      return -1;
    }
    cv_sleep(&cv_ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since boot.
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
sys_map(void)
{
  uint addr;
  uint len;

  if (argint(0, (int*) &addr) < 0)
    return -1;
  if (argint(1, (int*) &len) < 0)
    return -1;

  struct vmnode *vmn = vmn_allocpg(PGROUNDUP(len) / PGSIZE);
  if (vmn == 0)
    return -1;

  if (vmap_insert(proc->vmap, vmn, PGROUNDDOWN(addr)) < 0) {
    vmn_free(vmn);
    return -1;
  }

  return 0;
}

int
sys_unmap(void)
{
  uint addr;
  uint len;

  if (argint(0, (int*) &addr) < 0)
    return -1;
  if (argint(1, (int*) &len) < 0)
    return -1;
  if (vmap_remove(proc->vmap, PGROUNDDOWN(addr), PGROUNDUP(len)) < 0)
    return -1;

  updatepages(proc->vmap->pgdir,
	     (void*) (PGROUNDDOWN(addr)),
	     (void*) (PGROUNDDOWN(addr)+PGROUNDUP(len)), 0);
  cli();
  lcr3(v2p(proc->vmap->pgdir));
  for (uint i = 0; i < ncpu; i++)
    if (i != cpu->id)
      lapic_tlbflush(i);
  sti();
  return 0;
}
#endif

int
sys_halt(void)
{
  int i;
  const char s[] = "Shutdown";

  for(i = 0; i < 8; i++)
    outb(0x8900, s[i]);
  return 0;
}
