# Context switch
#
#   void swtch(struct context **old, struct context *new);
# 
# Save the current registers on the stack, creating
# a struct context, and save its address in *old.
# Switch stacks to new and pop previously-saved registers.

.globl swtch
swtch:
  # Save old callee-saved registers
  push %rbp
  push %rbx
  push %r11
  push %r12
  push %r13
  push %r14
  push %r15

  # Switch stacks
  mov %rsp, (%rdi)   # first arg of swtch is in rdi
  mov %rsi, %rsp     # second arg of swtch is in rsi

  # Load new callee-saved registers
  pop %r15
  pop %r14
  pop %r13
  pop %r12
  pop %r11
  pop %rbx
  pop %rbp

  ret

	
