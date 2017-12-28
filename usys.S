#include "syscall.h"
#include "traps.h"

#these are the basic calls for system functions - these are the calls made from user mode
#the parmeters given from the user are pushed to the stack and retrieved in the sys_*** implementation
# - moving the "System call number" into eax and doing interupt

#define SYSCALL(name) \
  .globl name; \
  name: \
    movl $SYS_ ## name, %eax; \
    int $T_SYSCALL; \
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
SYSCALL(history)
SYSCALL(wait2)
SYSCALL(set_prio)
SYSCALL(yield)																
