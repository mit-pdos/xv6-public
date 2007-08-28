#include "types.h"
#include "defs.h"
#include "param.h"
#include "mmu.h"
#include "proc.h"
#include "x86.h"

static void bootothers(void);

// Bootstrap processor starts running C code here.
int
main(void)
{
  int i;
  static volatile int bcpu;  // cannot be on stack
  extern char edata[], end[];

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
  bootothers();    // boot other CPUs
  if(!ismp)
    timer_init(); // uniprocessor timer
  userinit();      // first user process

  // enable interrupts on this processor.
  cpus[cpu()].nlock--;
  sti();

  scheduler();
}

// Additional processors start here.
static void
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

static void
bootothers(void)
{
  extern uchar _binary_bootother_start[], _binary_bootother_size[];
  uchar *code;
  struct cpu *c;

  // Write bootstrap code to unused memory at 0x7000.
  code = (uchar*)0x7000;
  memmove(code, _binary_bootother_start, (uint)_binary_bootother_size);

  for(c = cpus; c < cpus+ncpu; c++){
    if(c == cpus+cpu())  // We've started already.
      continue;

    // Fill in %esp, %eip and start code on cpu.
    *(void**)(code-4) = c->mpstack + MPSTACK;
    *(void**)(code-8) = mpmain;
    lapic_startap(c->apicid, (uint)code);

    // Wait for cpu to get through bootstrap.
    while(c->booted == 0)
      ;
  }
}

