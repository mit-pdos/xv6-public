# Context switch
#
#   void swtch(struct context **old, struct context *new);
#
# Save current register context in old
# and then load register context from new.

.global swtch
swtch:
  # Save old callee-save registers
  pushq   %rbp
  pushq   %rbx
  pushq   %r12
  pushq   %r13
  pushq   %r14
  pushq   %r15

  # Switch stacks
  movq    %rsp, (%rdi)
  movq    %rsi, %rsp

  # Load new callee-save registers
  popq    %r15
  popq    %r14
  popq    %r13
  popq    %r12
  popq    %rbx
  popq    %rbp

  retq #??
