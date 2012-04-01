#include "mmu.h"
#include "asmdefines.h"

.code64
.globl __fetchint64
.align 8
// rdi user src
// rsi kernel dst
// We aren't allowed to touch rbx,rsp,rbp,r12-r15
__fetchint64:
        mov     %gs:0x8, %r11
        movl    $1, PROC_UACCESS(%r11)
        mov     (%rdi), %r10
        mov     %r10, (%rsi)
        mov     $0, %rax
        jmp     __fetch_end
        
.globl __fetchstr
.align 8
// rdi kernel dst
// rsi user src
// rdx kernel len
// We aren't allowed to touch rbx,rsp,rbp,r12-r15
__fetchstr:
        mov     %gs:0x8, %r11
        movl    $1, PROC_UACCESS(%r11)

        // %rcx is loop instruction counter
        mov     %rdx, %rcx
        xor     %rax, %rax
1:   
        movb    (%rsi), %r10b
        movb    %r10b, (%rdi)
        // Check for NULL
        cmp     $0, %r10b
        je      2f
        inc     %rdi
        inc     %rsi
        loop    1b
        // Error
        movq    $-1, %rax
2:      // Done
        jmp     __fetch_end

.globl __fetch_end
.align 8
__fetch_end:
        movl $0, PROC_UACCESS(%r11)
        ret

.globl clear_page
.align 8
clear_page:
        movl $4096/8,%ecx
	xorl %eax,%eax
	rep stosq
	ret
