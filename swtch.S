#   void swtch(struct context *old, struct context *new);
#  
# Save current register context in old
# and then load register context from new.

.globl swtch
swtch:
  # Save old registers
  movl 4(%esp), %eax

  popl 0(%eax)  # %eip
  movl %esp, 4(%eax)
  movl %ebx, 8(%eax)
  movl %ecx, 12(%eax)
  movl %edx, 16(%eax)
  movl %esi, 20(%eax)
  movl %edi, 24(%eax)
  movl %ebp, 28(%eax)

  # Load new registers
  movl 4(%esp), %eax  # not 8(%esp) - popped return address above

  movl 28(%eax), %ebp
  movl 24(%eax), %edi
  movl 20(%eax), %esi
  movl 16(%eax), %edx
  movl 12(%eax), %ecx
  movl 8(%eax), %ebx
  movl 4(%eax), %esp
  pushl 0(%eax)  # %eip

  ret
