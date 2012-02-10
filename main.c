#include "types.h"
#include "multiboot.h"
#include "kernel.h"
#include "cpu.h"
#include "amd64.h"

static volatile int bstate;

void
mpboot(void)
{
  initseg();
  inittls();
  initlapic();
  initsamp();
  bstate = 1;
  scheduler();     // start running processes
}

static void
bootothers(void)
{
  extern u8 _bootother_start[];
  extern u64 _bootother_size;
  extern void (*apstart)(void);
  struct cpu *c;
  char *stack;
  u8 *code;

  // Write bootstrap code to unused memory at 0x7000.
  // The linker has placed the image of bootother.S in
  // _binary_bootother_start.
  code = p2v(0x7000);
  memmove(code, _bootother_start, _bootother_size);

  for(c = cpus; c < cpus+ncpu; c++){
    if(c == cpus+cpunum())  // We've started already.
      continue;

    // Tell bootother.S what stack to use and the address of apstart;
    // it expects to find these two addresses stored just before
    // its first instruction.
    stack = ksalloc(slab_stack);
    *(u32*)(code-4) = (u32)v2p(&apstart);
    *(u64*)(code-12) = (u64)stack + KSTACKSIZE;

    bstate = 0;
    lapicstartap(c->id, v2p(code));
    // Wait for cpu to finish mpmain()
    while(bstate == 0)
      ;
  }
}

void
cmain(u64 mbmagic, u64 mbaddr)
{
  extern pml4e_t kpml4[];
  extern u64 cpuhz;

  initpg();
  initseg();
  inittls();       // thread local storage
  inithz();        // CPU Hz, microdelay
  initpic();       // interrupt controller
  initioapic();
  inituart();
  initcga();
  initconsole();
  inittrap();
  initmp();
  initlapic();
  initkalloc(mbaddr);
  initproc();      // process table
  initsched();     // scheduler run queues
  initgc();        // gc epochs and threads
  initbio();       // buffer cache
  initinode();     // inode cache
  initdisk();      // disk
#if CILKENABLE
  initcilk();
#endif
  initsamp();
  initlockstat();
  initpci();
  initnet();

  if (VERBOSE)
    cprintf("ncpu %d %lu MHz\n", ncpu, cpuhz / 1000000);

  inituser();      // first user process
  bootothers();    // start other processors
  kpml4[0] = 0;    // don't need 1 GB identity mapping anymore
  lcr3(rcr3());

  scheduler();
  panic("Unreachable");
}
