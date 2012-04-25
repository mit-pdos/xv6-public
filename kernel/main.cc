#include "types.h"
#include "multiboot.hh"
#include "kernel.hh"
#include "spinlock.h"
#include "kalloc.hh"
#include "cpu.hh"
#include "amd64.h"
#include "hwvm.hh"
#include "condvar.h"
#include "proc.hh"
#include "apic.hh"

void initpic(void);
void initioapic(void);
void inituart(void);
void initcga(void);
void initconsole(void);
void initpg(void);
void initmp(void);
void inittls(void);
void initacpi(void);
void initnmi(void);
void inittrap(void);
void initseg(void);
void initkalloc(u64 mbaddr);
void initz(void);
void initrcu(void);
void initproc(void);
void initbio(void);
void initinode(void);
void initdisk(void);
void inituser(void);
void initsamp(void);
void initpci(void);
void initnet(void);
void initsched(void);
void initlockstat(void);
void initwq(void);
void initidle(void);
void initcpprt(void);
void initfutex(void);
void initcmdline(void);
void idleloop(void);

#define IO_RTC  0x70

static volatile int bstate;

void
mpboot(void)
{
  inittls();
  initseg();
  initlapic();
  initsamp();
  initidle();
  initnmi();
  bstate = 1;
  idleloop();
}

static void
warmreset(u32 addr)
{
  volatile u16 *wrv;

  // "The BSP must initialize CMOS shutdown code to 0AH
  // and the warm reset vector (DWORD based at 40:67) to point at
  // the AP startup code prior to the [universal startup algorithm]."
  outb(IO_RTC, 0xF);  // offset 0xF is shutdown code
  outb(IO_RTC+1, 0x0A);
  wrv = (u16*)(0x40<<4 | 0x67);  // Warm reset vector
  wrv[0] = 0;
  wrv[1] = addr >> 4;
}

static void
rstrreset(void)
{
  volatile u16 *wrv;
  
  // Paranoid: set warm reset code and vector back to defaults
  outb(IO_RTC, 0xF);
  outb(IO_RTC+1, 0);
  wrv = (u16*)(0x40<<4 | 0x67);
  wrv[0] = 0;
  wrv[1] = 0;
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
  code = (u8*) p2v(0x7000);
  memmove(code, _bootother_start, _bootother_size);

  for(c = cpus; c < cpus+ncpu; c++){
    if(c == cpus+myid())  // We've started already.
      continue;

    warmreset(v2p(code));

    // Tell bootother.S what stack to use and the address of apstart;
    // it expects to find these two addresses stored just before
    // its first instruction.
    stack = (char*) ksalloc(slab_stack);
    *(u32*)(code-4) = (u32)v2p(&apstart);
    *(u64*)(code-12) = (u64)stack + KSTACKSIZE;
    // bootother.S sets this to 0x0a55face early on
    *(u32*)(code-64) = 0;

    bstate = 0;
    lapicstartap(c->hwid, v2p(code));
    // Wait for cpu to finish mpmain()
    while(bstate == 0)
      ;
    rstrreset();
  }
}

void
cmain(u64 mbmagic, u64 mbaddr)
{
  extern u64 cpuhz;

  initpg();
  inithz();        // CPU Hz, microdelay
  initpic();       // interrupt controller
  initioapic();
  inituart();
  initcga();
  initmp();
  inittls();       // thread local storage

  // Call global constructors require mycpu()->id, which
  // we setup in inittls
  extern const uptr sctors[], ectors[];
  for (const uptr *ctorva = ectors; ctorva > sctors; ) {
    ((void(*)()) *--ctorva)();
  }

  initacpi();

  initseg();
  inittrap();
  initlapic();
  initcmdline();
  initkalloc(mbaddr);
  initwq();        // (after kalloc)
  initz();         // (after wq)
  initproc();      // process table
  initsched();     // scheduler run queues
  initidle();
  initgc();        // gc epochs and threads
  initdisk();      // disk
  initbio();       // buffer cache
  initinode();     // inode cache
  initconsole();
  initfutex();
  initsamp();
  initlockstat();
  initpci();
  initnet();

  if (VERBOSE)
    cprintf("ncpu %d %lu MHz\n", ncpu, cpuhz / 1000000);

  inituser();      // first user process
  initnmi();
  bootothers();    // start other processors
  kpml4.e[0] = 0;  // don't need 1 GB identity mapping anymore
  lcr3(rcr3());
  initcpprt();

  idleloop();

  panic("Unreachable");
}

void
halt(void)
{
  int i;
  const char s[] = "Shutdown";

  for(i = 0; i < 8; i++)
    outb(0x8900, s[i]);
  for (;;);
}
