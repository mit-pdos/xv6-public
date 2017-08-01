#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "x86.h"

static void startothers(void);
static void mpmain(void)  __attribute__((noreturn));
extern pde_t *kpgdir;
extern char end[]; // first address after kernel loaded from ELF file

// Bootstrap processor starts running C code here.
// Allocate a real stack and switch to it, first
// doing some setup required for memory allocator to work.
int
main(void)
{
  uartearlyinit();
  kinit1(end, P2V(4*1024*1024)); // phys page allocator
  kvmalloc();      // kernel page table
  //if(acpiinit())
  mpinit();        // detect other processors
  lapicinit();     // interrupt controller
  seginit();       // segment descriptors
  cprintf("\ncpu%d: starting xv6\n\n", cpunum());
  picinit();       // another interrupt controller
  ioapicinit();    // another interrupt controller
  consoleinit();   // console hardware
  uartinit();      // serial port
  pinit();         // process table
  tvinit();        // trap vectors
  binit();         // buffer cache
  fileinit();      // file table
  ideinit();       // disk
  if(!ismp)
    timerinit();   // uniprocessor timer
  startothers();   // start other processors
  kinit2(P2V(4*1024*1024), P2V(PHYSTOP)); // must come after startothers()
  userinit();      // first user process
  mpmain();        // finish this processor's setup
}

// Other CPUs jump here from entryother.S.
void
mpenter(void)
{
  switchkvm();
  seginit();
  lapicinit();
  mpmain();
}

// Common CPU setup code.
static void
mpmain(void)
{
  cprintf("cpu%d: starting\n", cpunum());
  idtinit();       // load idt register
  xchg(&cpu->started, 1); // tell startothers() we're up
  scheduler();     // start running processes
}

pde_t PML4T[NPDENTRIES];  // For entry.S
pde_t PDPTA[NPDENTRIES];  // For entry.S
pde_t PDPTB[NPDENTRIES];  // For entry.S
pde_t PDT[NPDENTRIES];  // For entry.S

void entry32mp(void);

// Start the non-boot (AP) processors.
static void
startothers(void)
{
  extern uchar _binary_entryother_start[], _binary_entryother_size[];
  uchar *code;
  struct cpu *c;
  char *stack;

  // Write entry code to unused memory at 0x7000.
  // The linker has placed the image of entryother.S in
  // _binary_entryother_start.
  code = P2V(0x7000);
  memmove(code, _binary_entryother_start, (addr_t)_binary_entryother_size);

  for(c = cpus; c < cpus+ncpu; c++){
    if(c == cpus+cpunum())  // We've started already.
      continue;

    // Tell entryother.S what stack to use, where to enter, and what
    // pgdir to use. We cannot use kpgdir yet, because the AP processor
    // is running in low  memory, so we use entrypgdir for the APs too.
    stack = kalloc();
    *(uint32*)(code-4) = 0x8000; // just enough stack to get us to entry64mp
    *(uint32*)(code-8) = v2p(entry32mp);
    *(uint64*)(code-16) = (uint64) (stack + KSTACKSIZE);



    lapicstartap(c->apicid, V2P(code));

    // wait for cpu to finish mpmain()
    while(c->started == 0)
      ;
  }
}

// The boot page table used in entry.S and entryother.S.
// Page directories (and page tables) must start on page boundaries,
// hence the __aligned__ attribute.
// PTE_PS in a page directory entry enables 4Mbyte pages.
/*
__attribute__((__aligned__(PGSIZE)))
pde_t entrypgdir[NPDENTRIES] = {
  // Map VA's [0, 4MB) to PA's [0, 4MB)
  [0] = (0) | PTE_P | PTE_W | PTE_PS,
  // Map VA's [KERNBASE, KERNBASE+4MB) to PA's [0, 4MB)
  
  [KERNBASE>>PDXSHIFT] = (0) | PTE_P | PTE_W | PTE_PS,
};*/


__attribute__((__aligned__(PGSIZE)))
pde_t PML4T[NPDENTRIES] = {
  // Map VA's [0, 4MB) to PA's [0, 4MB)
//  [0] = (PDPTA) | (pde_t*)PTE_P | (pde_t*)PTE_W,
//  [256] = (PDPTA) | (pde_t*)PTE_P | (pde_t*)PTE_W,
  
};

__attribute__((__aligned__(PGSIZE)))
pde_t PDPT[NPDENTRIES] = {
  // Map VA's [0, 4MB) to PA's [0, 4MB)
//  [0] = PDT | (0) | PTE_P | PTE_W,
};


__attribute__((__aligned__(PGSIZE)))
pde_t PDT[NPDENTRIES] = {
  // Map VA's [0, 4MB) to PA's [0, 4MB)
//  [0] = (0) | PTE_P | PTE_W,
  // Map VA's [KERNBASE, KERNBASE+4MB) to PA's [0, 4MB)
  
  //[1] = (0) | PTE_P | PTE_W,
};

/*__attribute__((__aligned__(PGSIZE)))
pde_t PT[NPDENTRIES] = {  ///hard code as 0x4000?
  // Map VA's [0, 4MB) to PA's [0, 4MB)
  [0] = (0) | PTE_P | PTE_W,
  // Map VA's [KERNBASE, KERNBASE+4MB) to PA's [0, 4MB)
  
  [1] = (0) | PTE_P | PTE_W,
};*/

//PAGEBREAK!
// Blank page.
//PAGEBREAK!
