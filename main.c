#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "x86.h"

static void enterothers(void);
static void mpmain(void)  __attribute__((noreturn));
extern pde_t *kpgdir;

// Bootstrap processor starts running C code here.
// Allocate a real stack and switch to it, first
// doing some setup required for memory allocator to work.
int
main(void)
{
  kvmalloc();      // kernel page table
  mpinit();        // collect info about this machine
  lapicinit(mpbcpu());
  seginit();       // set up segments
  cprintf("\ncpu%d: starting xv6\n\n", cpu->id);
  picinit();       // interrupt controller
  ioapicinit();    // another interrupt controller
  consoleinit();   // I/O devices & their interrupts
  uartinit();      // serial port
  pinit();         // process table
  tvinit();        // trap vectors
  binit();         // buffer cache
  fileinit();      // file table
  iinit();         // inode cache
  ideinit();       // disk
  if(!ismp)
    timerinit();   // uniprocessor timer
  enterothers();    // start other processors (must come before kinit; must use boot_alloc)
  kinit();         // initialize memory allocator
  userinit();      // first user process  (must come after kinit)
  // Finish setting up this processor in mpmain.
  mpmain();
}

// Other CPUs jump here from entryother.S.
static void
mpboot(void)
{
  switchkvm(); 
  seginit();
  lapicinit(cpunum());
  mpmain();
}

// Common CPU setup code.
static void
mpmain(void)
{
  cprintf("cpu%d: starting\n", cpu->id);
  idtinit();       // load idt register
  xchg(&cpu->booted, 1); // tell enterothers() we're up
  scheduler();     // start running processes
}

pde_t bootpgdir[];

// Start the non-boot processors.
static void
enterothers(void)
{
  extern uchar _binary_entryother_start[], _binary_entryother_size[];
  uchar *code;
  struct cpu *c;
  char *stack;

  // Write bootstrap code to unused memory at 0x7000.
  // The linker has placed the image of entryother.S in
  // _binary_entryother_start.
  code = p2v(0x7000);
  memmove(code, _binary_entryother_start, (uint)_binary_entryother_size);

  for(c = cpus; c < cpus+ncpu; c++){
    if(c == cpus+cpunum())  // We've started already.
      continue;

    // Tell entryother.S what stack to use, the address of mpboot and pgdir;
    // We cannot use kpgdir yet, because the AP processor is running in low 
    // memory, so we use bootpgdir for the APs too.  kalloc can return addresses
    // above 4Mbyte (the machine may have much more physical memory than 4Mbyte), which 
    // aren't mapped by bootpgdir, so we must allocate a stack using boot_alloc();
    // This introduces the constraint that xv6 cannot use kalloc until after these 
    // last boot_alloc invocations.
    stack = boot_alloc();
    *(void**)(code-4) = stack + KSTACKSIZE;
    *(void**)(code-8) = mpboot;
    *(int**)(code-12) = (void *) v2p(bootpgdir);

    lapicstartap(c->id, v2p(code));

    // wait for cpu to finish mpmain()
    while(c->booted == 0)
      ;
  }
}

// Boot page table used in multiboot.S and entryother.S.
// Page directories (and page tables), must start on a page boundary,
// hence the "__aligned__" attribute.  Also, because of restrictions
// related to linking and static initializers, we use "x + PTE_P"
// here, rather than the more standard "x | PTE_P".  Everywhere else
// you should use "|" to combine flags.
// Use PTE_PS in page directory entry to enable 4Mbyte pages.
pte_t dev_pgtable[NPTENTRIES];
pte_t entry_pgtable[NPTENTRIES];

__attribute__((__aligned__(PGSIZE)))
pde_t bootpgdir[NPDENTRIES] = {
	// Map VA's [0, 4MB) to PA's [0, 4MB)
	[0]
		= (0) + PTE_P + PTE_W + PTE_PS,
	// Map VA's [KERNBASE, KERNBASE+4MB) to PA's [0, 4MB)
	[KERNBASE>>PDXSHIFT]
    	        = (0) + PTE_P + PTE_W + PTE_PS,
};

//PAGEBREAK!
// Blank page.

