#include "types.h"
#include "param.h"
#include "multiboot.h"
#include "kernel.h"
#include "cpu.h"

extern void initpic(void);
extern void initioapic(void);
extern void inituart(void);
extern void initcga(void);
extern void initconsole(void);
extern void initpg(void);
extern void initmp(void);
extern void initlapic(void);
extern void inittls(void);
extern void inittrap(void);
extern void initseg(void);
extern void initkalloc(void);
extern void initrcu(void);
extern void initproc(void);
extern void initbio(void);
extern void initinode(void);
extern void initdisk(void);
extern void inituser(void);

static volatile int bstate;

// Common CPU setup code.
// Bootstrap CPU comes here from mainc().
// Other CPUs jump here from bootother.S.
void
mpboot(void)
{
  initseg();
  inittls();
  initlapic();
  bstate = 1;
  scheduler();     // start running processes
}

// Start the non-boot processors.
static void
bootothers(void)
{
  extern u8 _binary_bootother_start[], _binary_bootother_size[];
  extern void (*apstart)(void);
  struct cpu *c;
  char *stack;
  u8 *code;

  // Write bootstrap code to unused memory at 0x7000.
  // The linker has placed the image of bootother.S in
  // _binary_bootother_start.
  code = p2v(0x7000);
  memmove(code, _binary_bootother_start, (u64)_binary_bootother_size);

  for(c = cpus; c < cpus+ncpu; c++){
    if(c == cpus+cpunum())  // We've started already.
      continue;

    // Tell bootother.S what stack to use and the address of apstart;
    // it expects to find these two addresses stored just before
    // its first instruction.
    stack = ksalloc();
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
cmain(void)
{
  extern pml4e_t kpml4[];

  initpg();
  initseg();
  inittls();
  initpic();       // interrupt controller
  initioapic();
  inituart();
  initcga();
  initconsole();
  inittrap();
  initmp();
  initlapic();

  initkalloc();
  initrcu();       // initialize rcu module
  initproc();      // process table
  initbio();       // buffer cache
  initinode();     // inode cache
  initdisk();      // disk

  cprintf("ncpu %d\n", ncpu);

  inituser();      // first user process
  bootothers();    // start other processors
  kpml4[0] = 0;    // don't need 1 GB identity mapping anymore

  scheduler();
  panic("Unreachable");
}
