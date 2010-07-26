#include "param.h"
#include "types.h"
#include "defs.h"
#include "x86.h"
#include "mmu.h"
#include "proc.h"
#include "elf.h"

// The mappings from logical to linear are one to one (i.e.,
// segmentation doesn't do anything).
// The mapping from linear to physical are one to one for the kernel.
// The mappings for the kernel include all of physical memory (until
// PHYSTOP), including the I/O hole, and the top of physical address
// space, where additional devices are located.
// The kernel itself is linked to be at 1MB, and its physical memory
// is also at 1MB.
// Physical memory for user programs is allocated from physical memory
// between kernend and the end of physical memory (PHYSTOP).
// The virtual address space of each user program includes the kernel
// (which is inaccessible in user mode).  The user program addresses
// range from 0 till 640KB (USERTOP), which where the I/O hole starts
// (both in physical memory and in the kernel's virtual address
// space).

#define PHYSTOP  0x300000
#define USERTOP  0xA0000

static uint kerntext;  // Linker start kernel at 1MB
static uint kerntsz;   
static uint kerndata;
static uint kerndsz;
static uint kernend;
static uint freesz;

pde_t *kpgdir;         // One kernel page table for scheduler procs

void
printstack()
{
  uint *ebp = (uint *) rebp();
  uint i;
  cprintf("kernel stack: 0x%x\n", ebp);
  while (ebp) {
    if (ebp < (uint *) kerntext) // don't follow user ebp
      return;
    cprintf("  ebp %x  saved ebp %x eip %x  args", ebp, ebp[0], ebp[1]);
    for (i = 0; i < 4; i++)
      cprintf(" %x", ebp[2+i]);
    cprintf("\n");
    ebp = (uint *) ebp[0];
  }
}

void
printpgdir(pde_t *pgdir)
{
  uint i;
  uint j;

  cprintf("printpgdir 0x%x\n", pgdir);
  for (i = 0; i < NPDENTRIES; i++) {
    if (pgdir[i] != 0 && i < 100) {
      cprintf("pgdir %d, v=0x%x\n", i, pgdir[i]);
      pte_t *pgtab = (pte_t*) PTE_ADDR(pgdir[i]);
      for (j = 0; j < NPTENTRIES; j++) {
	if (pgtab[j] != 0)
	  cprintf("pgtab %d, v=0x%x, addr=0x%x\n", j, PGADDR(i, j, 0), 
		PTE_ADDR(pgtab[j]));
      }
    }
  }
  cprintf("printpgdir done\n", pgdir);
}

static pte_t *
walkpgdir(pde_t *pgdir, const void *va, int create)
{
  uint r;
  pde_t *pde;
  pte_t *pgtab;

  pde = &pgdir[PDX(va)];
  if (*pde & PTE_P) {
    pgtab = (pte_t*) PTE_ADDR(*pde);
  } else if (!create || !(r = (uint) kalloc(PGSIZE)))
    return 0;
  else {
    pgtab = (pte_t*) r;

    // Make sure all those PTE_P bits are zero.
    memset(pgtab, 0, PGSIZE);

    // The permissions here are overly generous, but they can
    // be further restricted by the permissions in the page table 
    // entries, if necessary.
    *pde = PADDR(r) | PTE_P | PTE_W | PTE_U;
  }
  return &pgtab[PTX(va)];
}

static int
mappages(pde_t *pgdir, void *la, uint size, uint pa, int perm, int p)
{
  uint i;
  pte_t *pte;

  if (p) 
    cprintf("mappages: pgdir 0x%x la 0x%x sz %d(0x%x) pa 0x%x, perm 0x%x\n", 
	    pgdir, la, size, size, pa, perm);
  for (i = 0; i < size; i += PGSIZE) {
    if (!(pte = walkpgdir(pgdir, (void*)(la + i), 1)))
      return 0;
    *pte = (pa + i) | perm | PTE_P;
    if (p) cprintf("mappages 0x%x 0x%x pp %d\n", la+i, *pte, PPN(*pte));
  }
  return 1;
}

// Set up CPU's kernel segment descriptors.
// Run once at boot time on each CPU.
void
ksegment(void)
{
  struct cpu *c;

  // Map once virtual addresses to linear addresses using identity map
  c = &cpus[cpunum()];
  c->gdt[SEG_KCODE] = SEG(STA_X|STA_R, 0, 0xffffffff, 0);
  c->gdt[SEG_KDATA] = SEG(STA_W, 0, 0xffffffff, 0);
  c->gdt[SEG_UCODE] = SEG(STA_X|STA_R, 0x0, 0xffffffff, DPL_USER);
  c->gdt[SEG_UDATA] = SEG(STA_W, 0x0, 0xffffffff, DPL_USER);

  // map cpu, and curproc
  c->gdt[SEG_KCPU] = SEG(STA_W, &c->cpu, 8, 0);

  lgdt(c->gdt, sizeof(c->gdt));
  loadgs(SEG_KCPU << 3);
  
  // Initialize cpu-local storage.
  cpu = c;
  proc = 0;
}

// Setup address space and current process task state.
void
loadvm(struct proc *p)
{
  pushcli();

  // Setup TSS
  cpu->gdt[SEG_TSS] = SEG16(STS_T32A, &cpu->ts, sizeof(cpu->ts)-1, 0);
  cpu->gdt[SEG_TSS].s = 0;
  cpu->ts.ss0 = SEG_KDATA << 3;
  cpu->ts.esp0 = (uint)proc->kstack + KSTACKSIZE;
  ltr(SEG_TSS << 3);

  if (p->pgdir == 0)
    panic("loadvm: no pgdir\n");

  lcr3(PADDR(p->pgdir));  // switch to new address space
  popcli();

  // Conservatively flush other processor's TLBs  
  // XXX lazy--just 2 cpus, but xv6 doesn't need shootdown anyway.
  if (cpu->id == 0) lapic_tlbflush(1);
  else lapic_tlbflush(0);
}

// Setup kernel part of a page table. Linear adresses map one-to-one
// on physical addresses.
pde_t*
setupkvm(void)
{
  pde_t *pgdir;

  // Allocate page directory
  if (!(pgdir = (pde_t *) kalloc(PGSIZE)))
    return 0;
  memset(pgdir, 0, PGSIZE);
  // Map IO space from 640K to 1Mbyte
  if (!mappages(pgdir, (void *)USERTOP, 0x60000, USERTOP, PTE_W, 0))
    return 0;
  // Map kernel text from kern text addr read-only
  if (!mappages(pgdir, (void *) kerntext, kerntsz, kerntext, 0, 0))
    return 0;
  // Map kernel data form kern data addr R/W
  if (!mappages(pgdir, (void *) kerndata, kerndsz, kerndata, PTE_W, 0))
    return 0;
  // Map dynamically-allocated memory read/write (kernel stacks, user mem)
  if (!mappages(pgdir, (void *) kernend, freesz, PADDR(kernend), PTE_W, 0))
    return 0;
  // Map devices such as ioapic, lapic, ...
  if (!mappages(pgdir, (void *)0xFE000000, 0x2000000, 0xFE000000, PTE_W, 0))
    return 0;
  return pgdir;
}

char*
uva2ka(pde_t *pgdir, char *uva)
{    
  pte_t *pte = walkpgdir(pgdir, uva, 0);
  if (pte == 0) return 0;
  uint pa = PTE_ADDR(*pte);
  return (char *)pa;
}

int
allocuvm(pde_t *pgdir, char *addr, uint sz)
{
  uint i, n;
  char *mem;

  n = PGROUNDUP(sz);
  if (addr + n >= USERTOP)
    return 0;
  for (i = 0; i < n; i += PGSIZE) {
    if (!(mem = kalloc(PGSIZE))) {   // XXX cleanup what we did?
      return 0;
    }
    memset(mem, 0, PGSIZE);
    mappages(pgdir, addr + i, PGSIZE, PADDR(mem), PTE_W|PTE_U, 0);
  }
  return 1;
}

void
freevm(pde_t *pgdir)
{
  uint i, j, da;

  if (!pgdir)
    panic("freevm: no pgdir\n");
  for (i = 0; i < NPDENTRIES; i++) {
    da = PTE_ADDR(pgdir[i]);
    if (da != 0) {
      pte_t *pgtab = (pte_t*) da;
      for (j = 0; j < NPTENTRIES; j++) {
	if (pgtab[j] != 0) {
	  uint pa = PTE_ADDR(pgtab[j]);
	  uint va = PGADDR(i, j, 0);
	  if (va >= USERTOP)   // done with user part?
	    break;
	  kfree((void *) pa, PGSIZE);
	  pgtab[j] = 0;
	}
      }
      kfree((void *) da, PGSIZE);
      pgdir[i] = 0;
    }
  }
  kfree((void *) pgdir, PGSIZE);
}

int
loaduvm(pde_t *pgdir, char *addr, struct inode *ip, uint offset, uint sz)
{
  uint i, pa, n;
  pte_t *pte;

  if ((uint)addr % PGSIZE != 0)
    panic("loaduvm: addr must be page aligned\n");
  for (i = 0; i < sz; i += PGSIZE) {
    if (!(pte = walkpgdir(pgdir, addr+i, 0)))
      panic("loaduvm: address should exist\n");
    pa = PTE_ADDR(*pte);
    if (sz - i < PGSIZE) n = sz - i;
    else n = PGSIZE;
    if(readi(ip, (char *)pa, offset+i, n) != n)
      return 0;
  }
  return 1;
}

void
inituvm(pde_t *pgdir, char *addr, char *init, uint sz)
{
  uint i, pa, n, off;
  pte_t *pte;

  for (i = 0; i < sz; i += PGSIZE) {
    if (!(pte = walkpgdir(pgdir, (void *)(i+addr), 0)))
	panic("inituvm: pte should exist\n");
    off = (i+(uint)addr) % PGSIZE;
    pa = PTE_ADDR(*pte);
    if (sz - i < PGSIZE) n = sz - i;
    else n = PGSIZE;
    memmove((char *)pa+off, init+i, n);
  }
}

pde_t*
copyuvm(pde_t *pgdir, uint sz)
{
  pde_t *d = setupkvm();
  pte_t *pte;
  uint pa, i;
  char *mem;

  if (!d) return 0;
  for (i = 0; i < sz; i += PGSIZE) {
    if (!(pte = walkpgdir(pgdir, (void *)i, 0)))
      panic("copyuvm: pte should exist\n");
    pa = PTE_ADDR(*pte);
    if (!(mem = kalloc(PGSIZE)))
      return 0;
    memmove(mem, (char *)pa, PGSIZE);
    if (!mappages(d, (void *)i, PGSIZE, PADDR(mem), PTE_W|PTE_U, 0))
      return 0;
  }
  return d;
}

void
pminit(void)
{
  extern char end[];
  struct proghdr *ph;
  struct elfhdr *elf = (struct elfhdr*)0x10000;  // scratch space

  if (elf->magic != ELF_MAGIC || elf->phnum != 2)
    panic("pminit: need a text and data segment\n");

  ph = (struct proghdr*)((uchar*)elf + elf->phoff);
  kernend = ((uint)end + PGSIZE) & ~(PGSIZE-1);
  kerntext = ph[0].va;
  kerndata = ph[1].va;
  kerntsz = kerndata - kerntext;
  kerndsz = kernend - kerndata;
  freesz = PHYSTOP - kernend;

  cprintf("kerntext@0x%x(sz=0x%x), kerndata@0x%x(sz=0x%x), kernend 0x%x freesz = 0x%x\n", 
	  kerntext, kerntsz, kerndata, kerndsz, kernend, freesz);

  kinit((char *)kernend, freesz);
}

// Jump to mainc on a properly-allocated kernel stack
void
jkstack(void)
{
  char *kstack = kalloc(PGSIZE);
  if (!kstack)
    panic("jkstack\n");
  char *top = kstack + PGSIZE;
  jstack((uint) top);
}

// Allocate one page table for the machine for the kernel address space
void
kvmalloc(void)
{
  kpgdir = setupkvm();
}

// Turn on paging.
void
vminit(void)
{
  uint cr0;

  lcr3(PADDR(kpgdir));
  cr0 = rcr0();
  cr0 |= CR0_PE|CR0_PG|CR0_AM|CR0_WP|CR0_NE|CR0_TS|CR0_EM|CR0_MP;
  cr0 &= ~(CR0_TS|CR0_EM);
  lcr0(cr0);
}

