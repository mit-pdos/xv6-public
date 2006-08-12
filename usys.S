#include "syscall.h"
#include "traps.h"

#define STUB(name) \
	.globl name; \
	name: \
		movl $SYS_ ## name, %eax; \
		int $T_SYSCALL; \
		ret 

STUB(fork)
STUB(exit)
STUB(wait)
STUB(cons_putc)
STUB(pipe)
STUB(read)
STUB(write)
STUB(close)
STUB(block)
STUB(kill)
STUB(panic)
STUB(cons_puts)
STUB(exec)
STUB(open)
STUB(mknod)
STUB(unlink)
STUB(fstat)
