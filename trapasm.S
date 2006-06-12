	.text
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
