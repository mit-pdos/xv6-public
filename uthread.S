.globl forkt
forkt:
        pushq %rdi              ## new stack ptr
        pushq %rsi              ## function ptr
        movq $1, %rdi           ## flag for sys_fork
        callq fork
        popq %r10               ## function ptr
        popq %r11               ## new stack ptr
        cmpl $0, %eax           ## pid/tid
        jne 1f
        movq %r11, %rsp
        andq $(~0xf), %rsp      ## alignment for amd64 ABI
        movq %rax, %rdi
        call *%r10
        call exit
1:
        ret
