#include "mmu.h"
        
	.text
        .globl alltraps
        .globl trap
alltraps:
        /* vectors.S sends all traps here */
        pushl   %ds     # build
        pushl   %es     #  trap
        pushal          #  frame
        movl $16,%eax    # SEG_KDATA << 3
        movw %ax,%ds        #  kernel
        movw %ax,%es        #  segments
        pushl %esp      # pass pointer to this trapframe
        call    trap        #  and call trap()
        addl $4, %esp
        # return falls through to trapret...
        
        .globl trapret
	/*
         * a forked process RETs here
         * expects ESP to point to a Trapframe
         */
trapret:
        popal
        popl %es
        popl %ds
        addl $0x8, %esp /* trapno and errcode */
        iret

		
.globl 	acpu
acpu:
	.long 0
