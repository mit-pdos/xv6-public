
  # vectors.S sends all traps here.
.globl alltraps
alltraps:
  # Build trap frame.
  push %r15
  push %r14
  push %r13
  push %r12
  push %r11
  push %r10
  push %r9
  push %r8
  push %rdi
  push %rsi
  push %rbp
  push %rdx
  push %rcx
  push %rbx
  push %rax

  mov  %rsp, %rdi  # frame in arg1
  call trap

  # Return falls through to trapret...
.globl trapret
trapret:
  pop %rax
  pop %rbx
  pop %rcx
  pop %rdx
  pop %rbp
  pop %rsi
  pop %rdi
  pop %r8
  pop %r9
  pop %r10
  pop %r11
  pop %r12
  pop %r13
  pop %r14
  pop %r15

  # discard trapnum and errorcode
  add $16, %rsp
  iretq


  # syscall_entry jumps here
.globl syscalltrap
syscalltrap:
  # Build trap frame.
  push %r15
  push %r14
  push %r13
  push %r12
#  push %r11
  push $0
  push %r10
  push %r9
  push %r8
  push %rdi
  push %rsi
  push %rbp
  push %rdx
#  push %rcx
  push $0
  push %rbx
  push %rax
  
  mov  %rsp, %rdi  # frame in arg1

 # mov $0xfffffffffffffff8,%rax
 # mov %fs:(%rax), %rax
 # mov 0x10(%rax), %rax

 # mov %rax, %rsp

  call trap

  # Return falls through to trapret...
.globl syscalltrapret
syscalltrapret:
  pop %rax
  pop %rbx
  pop %rcx
  pop %rdx
  pop %rbp
  pop %rsi
  pop %rdi
  pop %r8
  pop %r9
  pop %r10
  pop %r11
  pop %r12
  pop %r13
  pop %r14
  pop %r15

  # discard trapnum and errorcode
  add $16, %rsp
  sysret


