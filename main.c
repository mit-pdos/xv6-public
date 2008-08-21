#include "types.h"
#include "defs.h"
#include "param.h"
#include "mmu.h"
#include "proc.h"
#include "x86.h"

static void bootothers(void);
static void mpmain(void) __attribute__((noreturn));

// Bootstrap processor starts running C code here.
int
main(void)
{
  extern char edata[], end[];

  // clear BSS
  memset(edata, 0, end - edata);

  mp_init(); // collect info about this machine
  lapic_init(mp_bcpu());
  cprintf("\ncpu%d: starting xv6\n\n", cpu());

  pinit();         // process table
  binit();         // buffer cache
  pic_init();      // interrupt controller
  ioapic_init();   // another interrupt controller
  kinit();         // physical memory allocator
  tvinit();        // trap vectors
  fileinit();      // file table
  iinit();         // inode cache
  console_init();  // I/O devices & their interrupts
  ide_init();      // disk
  if(!ismp)
    timer_init();  // uniprocessor timer
  userinit();      // first user process
  bootothers();    // start other processors

  // Finish setting up this processor in mpmain.
  mpmain();
}

// Bootstrap processor gets here after setting up the hardware.
// Additional processors start here.
static void
mpmain(void)
{
  cprintf("cpu%d: mpmain\n", cpu());
  idtinit();
  if(cpu() != mp_bcpu())
    lapic_init(cpu());
  setupsegs(0);
  xchg(&cpus[cpu()].booted, 1);

  cprintf("cpu%d: scheduling\n", cpu());
  scheduler();
}

static void
bootothers(void)
{
  extern uchar _binary_bootother_start[], _binary_bootother_size[];
  uchar *code;
  struct cpu *c;
  char *stack;

  // Write bootstrap code to unused memory at 0x7000.
  code = (uchar*)0x7000;
  memmove(code, _binary_bootother_start, (uint)_binary_bootother_size);

  for(c = cpus; c < cpus+ncpu; c++){
    if(c == cpus+cpu())  // We've started already.
      continue;

    // Fill in %esp, %eip and start code on cpu.
    stack = kalloc(KSTACKSIZE);
    *(void**)(code-4) = stack + KSTACKSIZE;
    *(void**)(code-8) = mpmain;
    lapic_startap(c->apicid, (uint)code);

    // Wait for cpu to get through bootstrap.
    while(c->booted == 0)
      ;
  }
}

