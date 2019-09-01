# vectors.S sends all traps here.
.global alltraps
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
.global trapret
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

  add $16, %rsp  # discard trapnum and errorcode
  iretq
#PAGEBREAK!

# syscall_entry jumps here after syscall instruction
.global syscall_trap
syscall_trap:  # Build trap frame.
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
.global syscall_trapret
syscall_trapret:
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

  add $40, %rsp  # discard trapnum, errorcode, rip, cs and rflags

  # to make sure we don't get any interrupts on the user stack while in
  # supervisor mode this is actually slightly unsafe still,
  # since some interrupts are nonmaskable.
  cli
  mov (%rsp), %rsp  # restore the user stack
  sysretq
