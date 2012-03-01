#include "syscall.h"
#include "traps.h"

#define SYSCALL_INT(name) \
  .globl name; \
  name: \
    movq $SYS_ ## name, %rax; \
    int $T_SYSCALL; \
    ret

#define SYSCALL(name) \
  .globl name; \
  name: \
    movq $SYS_ ## name, %rax; \
    movq %rcx, %r10; \
    syscall; \
    ret

SYSCALL(fork)
SYSCALL(exit)
SYSCALL(wait)
SYSCALL(pipe)
SYSCALL(read)
SYSCALL(write)
SYSCALL(close)
SYSCALL(kill)
SYSCALL_INT(exec)
SYSCALL(openat)
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
SYSCALL(socket)
SYSCALL(bind)
SYSCALL(listen)
SYSCALL(accept)
SYSCALL(pread)
SYSCALL(async)
SYSCALL(script)
SYSCALL(setfs)
