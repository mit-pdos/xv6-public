#include "types.h"
#include "memlayout.h"
#include "riscv.h"
#include "defs.h"

void main();

// entry.S uses this as the initial stack.
__attribute__ ((aligned (16))) char stack0[4096];

// assembly code in kernelvec for machine-mode timer interrupt.
extern void machinevec();

// scratch area for timer interrupt.
uint64 mscratch0[8];

__attribute__ ((aligned (16)))
void
xyzzy()
{
  uartputc('I');
}

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

  // set up to receive timer interrupts in machine mode.
  *(uint64*)CLINT_MTIMECMP0 = *(uint64*)CLINT_MTIME + 10000;
  mscratch0[4] = CLINT_MTIMECMP0;
  mscratch0[5] = 10000000;
  w_mscratch((uint64)mscratch0);
  w_mtvec((uint64)machinevec);
  w_mstatus(r_mstatus() | MSTATUS_MIE);
  w_mie(r_mie() |  MIE_MTIE);

  // jump to main in supervisor mode.
  asm("mret");
}
