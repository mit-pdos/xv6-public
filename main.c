#include "types.h"
#include "param.h"
#include "mmu.h"
#include "proc.h"
#include "defs.h"
#include "x86.h"
#include "traps.h"
#include "syscall.h"
#include "elf.h"
#include "param.h"
#include "spinlock.h"

extern char edata[], end[];

// Bootstrap processor starts running C code here.
// This is called main0 not main so that it can have
// a void return type.  Gcc can't handle functions named
// main that don't return int.  Really.
void
main0(void)
{
  int i;
  static volatile int bcpu;  // cannot be on stack

  // clear BSS
  memset(edata, 0, end - edata);

  // Prevent release() from enabling interrupts.
  for(i=0; i<NCPU; i++)
    cpus[i].nlock = 1;

  mp_init(); // collect info about this machine
  bcpu = mp_bcpu();

  // Switch to bootstrap processor's stack
  asm volatile("movl %0, %%esp" : : "r" (cpus[bcpu].mpstack+MPSTACK-32));
  asm volatile("movl %0, %%ebp" : : "r" (cpus[bcpu].mpstack+MPSTACK));

  lapic_init(bcpu);
  cprintf("\ncpu%d: starting xv6\n\n", cpu());

  pinit();         // process table
  binit();         // buffer cache
  pic_init();      // interrupt controller
  ioapic_init();   // another interrupt controller
  kinit();         // physical memory allocator
  tvinit();        // trap vectors
  idtinit();       // interrupt descriptor table
  fileinit();      // file table
  iinit();         // inode cache
  setupsegs(0);    // segments & TSS
  console_init();  // I/O devices & their interrupts
  ide_init();      // disk
  mp_startthem();  // other CPUs
  if(!ismp)
    pit8253_timerinit(); // uniprocessor timer
  userinit();      // first user process

  // enable interrupts on this processor.
  cpus[cpu()].nlock--;
  sti();

  scheduler();
}

// Additional processors start here.
void
mpmain(void)
{
  cprintf("cpu%d: starting\n", cpu());
  idtinit();
  lapic_init(cpu());
  setupsegs(0);

  cpuid(0, 0, 0, 0, 0);  // memory barrier
  cpus[cpu()].booted = 1;

  // Enable interrupts on this processor.
  cpus[cpu()].nlock--;
  sti();

  scheduler();
}

