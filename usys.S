#include "syscall.h"
#include "traps.h"

#define SYSCALL(name) \
  .globl name; \
  name: \
    pushq %r9; \
    pushq %r8; \
    pushq %rcx; \
    pushq %rdx; \
    pushq %rsi; \
    pushq %rdi; \
    pushq $0; \
    movq $SYS_ ## name, %rax; \
    int $T_SYSCALL; \
    addq $(7 * 8), %rsp; \
    ret

SYSCALL(fork)
SYSCALL(exit)
SYSCALL(wait)
SYSCALL(pipe)
SYSCALL(read)
SYSCALL(write)
SYSCALL(close)
SYSCALL(kill)
SYSCALL(exec)
SYSCALL(open)
SYSCALL(mknod)
SYSCALL(unlink)
SYSCALL(fstat)
SYSCALL(link)
SYSCALL(mkdir)
SYSCALL(chdir)
SYSCALL(dup)
SYSCALL(getpid)
SYSCALL(sbrk)
SYSCALL(sleep)
SYSCALL(uptime)
SYSCALL(map)
SYSCALL(unmap)
SYSCALL(halt)
