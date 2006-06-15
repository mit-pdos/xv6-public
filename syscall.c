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
  curproc->state = UNUSED;
  // XXX free resources. notify parent. abandon children.
  swtch();
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
  default:
    cprintf("unknown sys call %d\n", num);
    // XXX fault
    break;
  }
}
