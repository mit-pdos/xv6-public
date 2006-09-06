#include "types.h"
#include "stat.h"
#include "param.h"
#include "mmu.h"
#include "proc.h"
#include "defs.h"
#include "x86.h"
#include "traps.h"
#include "syscall.h"
#include "spinlock.h"
#include "buf.h"
#include "fs.h"
#include "fsvar.h"
#include "elf.h"
#include "fd.h"
#include "fcntl.h"

int
sys_fork(void)
{
  struct proc *np;

  if((np = copyproc(curproc[cpu()])) == 0)
    return -1;
  np->state = RUNNABLE;
  return np->pid;
}

int
sys_exit(void)
{
  proc_exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return proc_wait();
}

int
sys_kill(void)
{
  int pid;

  if(fetcharg(0, &pid) < 0)
    return -1;
  return proc_kill(pid);
}

int
sys_getpid(void)
{
  struct proc *cp = curproc[cpu()];
  return cp->pid;
}

int
sys_sbrk(void)
{
  uint addr;
  int n;
  struct proc *cp = curproc[cpu()];

  if(fetcharg(0, &n) < 0)
    return -1;
  if((addr = growproc(n)) == 0xffffffff)
    return -1;
  setupsegs(cp);
  return addr;
}
