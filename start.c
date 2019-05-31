#include "types.h"
#include "memlayout.h"
#include "riscv.h"
#include "defs.h"

void main();

// entry.S uses this as the initial stack.
char stack0[4096];

// entry.S jumps here in machine mode on stack0.
void
mstart()
{
  // set M Previous Privilege mode to Supervisor, for mret.
  unsigned long x = r_mstatus();
  x &= ~MSTATUS_MPP_MASK;
  x |= MSTATUS_MPP_S;
  w_mstatus(x);

  // set M Exception Program Counter to main, for mret.
  // requires gcc -mcmodel=medany
  w_mepc((uint64)main);

  // disable paging for now.
  w_satp(0);

  // delegate all interrupts and exceptions to supervisor mode.
  w_medeleg(0xffff);
  w_mideleg(0xffff);
  
  // jump to main in supervisor mode.
  asm("mret");
}
