#include "syscall.h"
#include "traps.h"

.globl forkt
forkt:
  movl 4(%esp), %ecx	## new stack ptr
  movl 8(%esp), %edx	## function ptr

  pushl $1		## flag for sys_fork
  pushl $0		## return address (dummy)
  movl $SYS_fork, %eax
  int $T_SYSCALL
  addl $8, %esp

  cmpl $0, %eax
  jne 1f

  movl %ecx, %esp
  jmp *%edx

1:
  ret

