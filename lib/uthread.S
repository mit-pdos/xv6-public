#include "syscall.h"
#include "traps.h"

# We assume that the kernel follows the amd64 ABI, but not
# that it saves caller-saved registers.
.globl forkt
forkt:
        pushq %r12
        andq $(~0xf), %rdi      # new stack ptr amd64 ABI alignment 
        movq %rdi, %r12
        movq %rdx, -16(%r12)    # arg
        movq %rsi, -8(%r12)     # function ptr

        movq $1, %rdi           # flag for sys_fork
        movq $SYS_fork, %rax
        syscall

        cmpq $0, %rax           # pid/tid
        jne 1f

        # child
        movq -8(%r12), %rax
        movq -16(%r12), %rdi
        movq %r12, %rsp
        call *%rax
        call exit
1:      # parent
        popq %r12
        ret
