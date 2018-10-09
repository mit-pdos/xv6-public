#include "param.h"
#include "x86.h"	
#include "mmu.h"

# the offset of cs in trapframe (i.e., tf->cs - tf)
#define CSOFF  144
	
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

  cmpw $SEG_KCODE, CSOFF(%rsp)   # compare to saved cs
  jz 1f 
  swapgs
  
1:mov  %rsp, %rdi  # frame in arg1
  call trap

# Return falls through to trapret...
.globl trapret
trapret:
  cli
  cmpw $SEG_KCODE, CSOFF(%rsp)  # compare to saved cs
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

# syscall jumps here after syscall instruction
.globl sysentry
sysentry:  # Build syscall frame.
  // load kernel stack address
  swapgs
  movq  %rax, %gs:0  // save %rax in syscallno of cpu entry
  movq  %rsp, %gs:8  // user sp
  movq  %gs:16, %rax  // proc entry
  
  movq  %ss:0(%rax), %rax // load kstack from proc
  addq  $(KSTACKSIZE), %rax

  movq  %rax, %rsp
  movq  %gs:0, %rax  // restore rax

  push %gs:8
  push %rcx
  push %r11	
  push %rax	

  push %rbp
  push %rbx	
  push %r12 
  push %r13
  push %r14
  push %r15	

  push %r9
  push %r8	
  push %r10
  push %rdx
  push %rsi	
  push %rdi
	
  mov  %rsp, %rdi  # frame in arg1

  call syscall
  # fall through to sysexit
	
.globl sysexit
sysexit:
  # to make sure we don't get any interrupts on the user stack while in
  # supervisor mode.  insufficient?  (see vunerability reports for sysret)
  cli
	
  pop %rdi
  pop %rsi
  pop %rdx
  pop %r10
  pop %r8
  pop %r9

  pop %r15
  pop %r14
  pop %r13
  pop %r12
  pop %rbx
  pop %rbp

  pop %rax
  pop %r11
  pop %rcx

  mov (%rsp),%rsp  # switch to the user stack
  # there are two more values on the stack, but we don't care about them	
  swapgs	

  sysretq

