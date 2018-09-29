#include "param.h"
#include "x86.h"	
#include "mmu.h"
	
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

  cmpw $KCSEG, 144(%rsp)   # compare to saved cs
  jz 1f 
  swapgs
  
1:mov  %rsp, %rdi  # frame in arg1
  call trap

# Return falls through to trapret...
.globl trapret
trapret:
  cli
  cmpw $KCSEG, 144(%rsp)  # compare to saved cs
  jz 1f
  swapgs

1:pop %rax
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
.globl sysentry
sysentry:  # Build trap frame.
  // load kernel stack address
  swapgs
  movq  %rax, %gs:0  // save %rax in syscallno of cpu entry
  movq  %rsp, %gs:8  // user sp
  movq  %gs:16, %rax  // proc entry
  
  movq  %ss:0(%rax), %rax // load kstack from proc
  addq  $(KSTACKSIZE), %rax

  movq  %rax, %rsp
  movq  %gs:0, %rax  // restore rax

  // push usp to make a valid trapframe
  push $(UDSEG|0x3)
  push %gs:8
  // safe eflags and eip
  push %r11
  push $(UCSEG|0x3)
  push %rcx
  // push errno and trapno to make stack look like a trap
  push $0
  push $64

  // push values on kernel stack
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
#PAGEBREAK!

# Return falls through to trapret...
.globl sysexit
sysexit:
  # to make sure we don't get any interrupts on the user stack while in
  # supervisor mode.  insufficient?  (see vunerability reports for sysret)
  cli
  
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

  add $(5*8), %rsp  # discard trapnum, errorcode, rip, cs and rflags
  mov (%rsp),%rsp  # switch to the user stack
  # there are two more values on the stack, but we don't care about them	
  swapgs	

  sysretq

