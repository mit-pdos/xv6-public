# We assume that the kernel follows the amd64 ABI, but not
# that it saves caller-saved registers.
.globl forkt
forkt:
        pushq %r12
        andq $(~0xf), %rdi      # new stack ptr amd64 ABI alignment 
        movq %rdi, %r12
        subq $0x10, %r12
        movq %rdx, 0x00(%r12)   # arg
        movq %rsi, 0x08(%r12)   # function ptr

        movq %rcx, %rdi         # flag for sys_fork
        movq $SYS_fork, %rax
        syscall

        cmpq $0, %rax           # pid/tid
        jne 1f

        # child
        movq %r12, %rsp
        movq %rax, %rdi
        call forkt_setup
        popq %rdi
        popq %rax
        call *%rax
        call exit
1:      # parent
        popq %r12
        ret
