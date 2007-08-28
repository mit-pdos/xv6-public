.text

.set SEG_KDATA_SEL, 0x10   # selector for SEG_KDATA

  # vectors.S sends all traps here.
.globl alltraps
alltraps:
  # Build trap frame.
  pushl %ds
  pushl %es
  pushal
  
  # Set up data segments.
  movl $SEG_KDATA_SEL, %eax
  movw %ax,%ds
  movw %ax,%es

  # Call trap(tf), where tf=%esp
  pushl %esp
  call trap
  addl $4, %esp

  # Return falls through to trapret...
.globl trapret
trapret:
  popal
  popl %es
  popl %ds
  addl $0x8, %esp  # trapno and errcode
  iret

  # A forked process switches to user mode by calling
  # forkret1(tf), where tf is the trap frame to use.
.globl forkret1
forkret1:
  movl 4(%esp), %esp
  jmp trapret

