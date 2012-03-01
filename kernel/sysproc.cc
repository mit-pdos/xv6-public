#include "types.h"
#include "amd64.h"
#include "kernel.hh"
#include "mmu.h"
#include "spinlock.h"
#include "condvar.h"
#include "queue.h"
#include "proc.hh"
#include "cpu.hh"
#include "vm.hh"
#include "sperf.hh"

long
sys_fork(int flags)
{
  return fork(flags);
}

long
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

long
sys_wait(void)
{
  return wait();
}

long
sys_kill(int pid)
{
  return kill(pid);
}

long
sys_getpid(void)
{
  return myproc()->pid;
}

long
sys_sbrk(int n)
{
  uptr addr;

  addr = myproc()->brk;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

long
sys_sleep(int n)
{
  u32 ticks0;
  
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
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
long
sys_uptime(void)
{
  u64 xticks;
  
  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

long
sys_map(uptr addr, u64 len)
{
  ANON_REGION(__func__, &perfgroup);

  vmnode *vmn = new vmnode(PGROUNDUP(len) / PGSIZE);
  if (vmn == 0)
    return -1;

  if (myproc()->vmap->insert(vmn, PGROUNDDOWN(addr), 1) < 0) {
    delete vmn;
    return -1;
  }

  return 0;
}

long
sys_unmap(uptr addr, u64 len)
{
  ANON_REGION(__func__, &perfgroup);

  uptr align_addr = PGROUNDDOWN(addr);
  uptr align_len = PGROUNDUP(addr + len) - align_addr;
  if (myproc()->vmap->remove(align_addr, align_len) < 0)
    return -1;

  return 0;
}

long
sys_halt(void)
{
  int i;
  const char s[] = "Shutdown";

  for(i = 0; i < 8; i++)
    outb(0x8900, s[i]);
  return 0;
}

long
sys_setfs(u64 base)
{
  proc *p = myproc();
  p->user_fs_ = base;
  switchvm(p);
  return 0;
}
