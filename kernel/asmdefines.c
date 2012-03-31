#include "types.h"
#include "kernel.hh"
#include "spinlock.h"
#include "amd64.h"
#include "condvar.h"
#include "proc.hh"
#include "cpu.hh"

#define DEFINE(sym, val) \
  asm volatile ("\n#define " #sym " remove%0 " : : "i" (val))

void
asmdefines(void)
{
  DEFINE(PROC_KSTACK_OFFSET, __offsetof(struct proc, kstack));
  DEFINE(TF_CS, __offsetof(struct trapframe, cs));
  DEFINE(PROC_UACCESS, __offsetof(struct proc, uaccess_));
  DEFINE(TRAPFRAME_SIZE, sizeof(trapframe));
}
