.code64
.align 8
.globl _start
_start:
        pop %rdi
        mov %rsp, %rsi
        call main
        call exit
