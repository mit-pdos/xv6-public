.globl setjmp
setjmp:
  movl 4(%esp), %eax
  
  movl %ebx, 0(%eax)
  movl %ecx, 4(%eax)
  movl %edx, 8(%eax)
  movl %esi, 12(%eax)
  movl %edi, 16(%eax)
  movl %esp, 20(%eax)
  movl %ebp, 24(%eax)
  pushl 0(%esp)   /* %eip */
  popl 28(%eax)
  
  movl $0, %eax   /* return value */
  ret

.globl longjmp
longjmp:
  movl 4(%esp), %eax
  
  movl 0(%eax), %ebx
  movl 4(%eax), %ecx
  movl 8(%eax), %edx
  movl 12(%eax), %esi
  movl 16(%eax), %edi
  movl 20(%eax), %esp
  movl 24(%eax), %ebp

  addl $4, %esp   /* pop %eip into thin air */
  pushl 28(%eax)  /* push new %eip */
  
  movl $1, %eax   /* return value (appears to come from setjmp!) */
  ret

