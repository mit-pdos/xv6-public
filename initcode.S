# Initial process execs /init.

#include "syscall.h"
#include "traps.h"

# exec(init, argv)
.globl start
start:
  pushq $argv
  pushq $init
  pushq $0  // where caller pc would be
  movq $SYS_exec, %rax
  int $T_SYSCALL

# for(;;) exit();
exit:
  movq $SYS_exit, %rax
  int $T_SYSCALL
  jmp exit

# char init[] = "/init\0";
init:
  .string "/init\0"

# char *argv[] = { init, 0 };
.p2align 2
argv:
  .quad init
  .quad 0

