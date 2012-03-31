#include "mmu.h"
#include "asmdefines.h"

.code64
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
        je      done
        inc     %rdi
        inc     %rsi
        loop    1b
        // Error
        movq    $-1, %rax
done:
        jmp __fetch_end

.code64
.globl __fetch_end
.align 8
__fetch_end:
        movl $0, PROC_UACCESS(%r11)
        ret
