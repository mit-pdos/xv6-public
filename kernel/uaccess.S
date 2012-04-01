#include "mmu.h"
#include "asmdefines.h"

#define ENTRY(name) .globl name ; .align 8; name :

// We aren't allowed to touch rbx,rsp,rbp,r12-r15
        
.code64
// rdi user src
// rsi kernel dst
ENTRY(__uaccess_int64)
        mov     %gs:0x8, %r11
        movl    $1, PROC_UACCESS(%r11)
        mov     (%rdi), %r10
        mov     %r10, (%rsi)
        mov     $0, %rax
        jmp     __uaccess_end

// rdi dst
// rsi src
// rdx dst len
ENTRY(__uaccess_str)
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
        jmp     __uaccess_end

// rdi dst
// rsi src
// rdx len
ENTRY(__uaccess_mem)
        mov     %gs:0x8, %r11
        movl    $1, PROC_UACCESS(%r11)

        // %rcx is loop instruction counter
        mov     %rdx, %rcx
        xor     %rax, %rax
1:   
        movb    (%rsi), %r10b
        movb    %r10b, (%rdi)
        inc     %rdi
        inc     %rsi
        loop    1b
        // Done
        jmp     __uaccess_end
        
.globl __uaccess_end
.align 8
__uaccess_end:
        movl $0, PROC_UACCESS(%r11)
        ret

.globl zpage
.align 8
zpage:
        movl $4096/8,%ecx
	xorl %eax,%eax
	rep stosq
	ret

// XXX(sbw) should skip first cache line, prefetchnt, then zero
.globl zrun_nc
.align 8
zrun_nc:
        jmp zpage
        