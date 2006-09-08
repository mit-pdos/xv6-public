#   int  setjmp(struct jmpbuf *jmp);
#   void longjmp(struct jmpbuf *jmp);
# 
# Setjmp saves its stack environment in jmp for later use by longjmp.
# It returns 0.
# 
# Longjmp restores the environment saved by the last call of setjmp.
# It then causes execution to continue as if the call of setjmp
# had just returned 1.
# 
# The caller of setjmp must not itself have returned in the interim.
# All accessible data have values as of the time longjmp was called.
#
#    [Description, but not code, borrowed from Plan 9.]

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
  pushl 0(%esp)   # %eip
  popl 28(%eax)

  movl $0, %eax   # return value
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

  addl $4, %esp   # pop and discard %eip
  pushl 28(%eax)  # push new %eip

  movl $1, %eax   # return value (appears to come from setjmp!)
  ret
