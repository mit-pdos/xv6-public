# vectors.S sends all traps here.
.global alltraps
alltraps:
  # Build trap frame.
  pushq   %r15
  pushq   %r14
  pushq   %r13
  pushq   %r12
  pushq   %r11
  pushq   %r10
  pushq   %r9
  pushq   %r8
  pushq   %rdi
  pushq   %rsi
  pushq   %rbp
  pushq   %rdx
  pushq   %rcx
  pushq   %rbx
  pushq   %rax

  movq    %rsp, %rdi  # frame in arg1
  callq   trap
# Return falls through to trapret...

.global trapret
trapret:
  popq    %rax
  popq    %rbx
  popq    %rcx
  popq    %rdx
  popq    %rbp
  popq    %rsi
  popq    %rdi
  popq    %r8
  popq    %r9
  popq    %r10
  popq    %r11
  popq    %r12
  popq    %r13
  popq    %r14
  popq    %r15

  addq    $16, %rsp  # discard trapnum and errorcode
  iretq
#PAGEBREAK!

.global syscall_entry
syscall_entry:
  # switch to kernel stack. With the syscall instruction,
  # this is a kernel resposibility
  # store %rsp on the top of proc->kstack,
  movq    %rax, %fs:(0)      # save %rax above __thread vars
  movq    %fs:(-8), %rax     # %fs:(-8) is proc (the last __thread)
  movq    0x10(%rax), %rax   # get proc->kstack (see struct proc)
  addq    $(4096-16), %rax   # %rax points to tf->rsp
  movq    %rsp, (%rax)       # save user rsp to tf->rsp
  movq    %rax, %rsp         # switch to the kstack
  movq    %fs:(0), %rax      # restore %rax

  pushq   %r11         # rflags
  pushq   $0           # cs is ignored
  pushq   %rcx         # rip (next user insn)

  pushq   $0           # err
  pushq   $0           # trapno ignored

  pushq   %r15
  pushq   %r14
  pushq   %r13
  pushq   %r12
  pushq   %r11
  pushq   %r10
  pushq   %r9
  pushq   %r8
  pushq   %rdi
  pushq   %rsi
  pushq   %rbp
  pushq   %rdx
  pushq   %rcx
  pushq   %rbx
  pushq   %rax

  movq    %rsp, %rdi  # frame in arg1
  callq   syscall
# Return falls through to syscall_trapret...
#PAGEBREAK!

.global syscall_trapret
syscall_trapret:
  popq    %rax
  popq    %rbx
  popq    %rcx
  popq    %rdx
  popq    %rbp
  popq    %rsi
  popq    %rdi
  popq    %r8
  popq    %r9
  popq    %r10
  popq    %r11
  popq    %r12
  popq    %r13
  popq    %r14
  popq    %r15

  addq    $40, %rsp  # discard trapnum, errorcode, rip, cs and rflags

  # to make sure we don't get any interrupts on the user stack while in
  # supervisor mode. this is actually slightly unsafe still,
  # since some interrupts are nonmaskable.
  # See https://www.felixcloutier.com/x86/sysret
  cli
  movq    (%rsp), %rsp  # restore the user stack
  sysretq
