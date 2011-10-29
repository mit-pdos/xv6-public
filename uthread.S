.globl forkt
forkt:
        pushq %rdi      ## new stack ptr
        pushq %rsi      ## function ptr
        movq $1, %rdi   ## flag for sys_fork
        callq fork
        popq %rsi
        popq %rdi
        cmpl $0, %eax
        jne 1f
        movq %rdi, %rsp
        jmp *%rsi
1:
        ret
