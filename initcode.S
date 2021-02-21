# Initial process execs /init.

#include "syscall.h"
#include "traps.h"

# exec(init, argv)
.code64
.global start
start:
  mov $init, %rdi
  mov $argv, %rsi
  mov $SYS_exec, %rax
  syscall

# for(;;) exit();
exit:
  mov $SYS_exit, %rax
  syscall
  jmp exit

# char init[] = "/init\0";
init:
  .string "/init\0"

# char *argv[] = { init, 0 };
.p2align 3
argv:
  .quad init
  .quad 0
