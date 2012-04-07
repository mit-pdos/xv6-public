# Initial process execs /init.

#include "traps.h"

# exec(init, argv)
.globl start
start:
  movq $argv, %rsi
  movq $init, %rdi
  movq $SYS_exec, %rax
  syscall

# for(;;) exit();
exit:
  movq $SYS_exit, %rax
  syscall
  jmp exit

# char init[] = "/init\0";
init:
  .string "/init\0"

# char *argv[] = { init, 0 };
.p2align 2
argv:
  .quad init
  .quad 0

