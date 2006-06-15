#include "types.h"
#include "param.h"
#include "mmu.h"
#include "proc.h"
#include "defs.h"
#include "x86.h"
#include "traps.h"
#include "syscall.h"

/*
 * User code makes a system call with INT T_SYSCALL.
 * System call number in %eax.
 * Arguments on the stack.
 *
 * Return value? Error indication? Errno?
 */

void
sys_fork()
{
  newproc();
}

void
sys_exit()
{
  struct proc *p;

  curproc->state = ZOMBIE;

  // wake up parent
  for(p = proc; p < &proc[NPROC]; p++)
    if(p->pid == curproc->ppid)
      wakeup(p);

  // abandon children
  for(p = proc; p < &proc[NPROC]; p++)
    if(p->ppid == curproc->pid)
      p->pid = 1;

  swtch();
}

void
sys_wait()
{
  struct proc *p;
  int any;

  cprintf("waid pid %d ppid %d\n", curproc->pid, curproc->ppid);

  while(1){
    any = 0;
    for(p = proc; p < &proc[NPROC]; p++){
      if(p->state == ZOMBIE && p->ppid == curproc->pid){
        kfree(p->mem, p->sz);
        kfree(p->kstack, KSTACKSIZE);
        p->state = UNUSED;
        cprintf("%x collected %x\n", curproc, p);
        return;
      }
      if(p->state != UNUSED && p->ppid == curproc->pid)
        any = 1;
    }
    if(any == 0){
      cprintf("%x nothing to wait for\n", curproc);
      return;
    }
    sleep(curproc);
  }
}

void
syscall()
{
  int num = curproc->tf->tf_regs.reg_eax;

  cprintf("%x sys %d\n", curproc, num);
  switch(num){
  case SYS_fork:
    sys_fork();
    break;
  case SYS_exit:
    sys_exit();
    break;
  case SYS_wait:
    sys_wait();
    break;
  default:
    cprintf("unknown sys call %d\n", num);
    // XXX fault
    break;
  }
}
