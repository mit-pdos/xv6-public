#include "param.h"
#include "types.h"
#include "defs.h"
#include "x86.h"
#include "msr.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "elf.h"
#include "traps.h"

extern char data[];  // defined by kernel.ld
void sysentry(void);

static pde_t *kpml4; // kernel address space, used by scheduler and bootup

// Bootstrap GDT.  Used by boot.S but defined in C
// Map "logical" addresses to virtual addresses using identity map.
// Cannot share a CODE descriptor for both kernel and user
// because it would have to have DPL_USR, but the CPU forbids
// an interrupt from CPL=0 to DPL=3.
struct segdesc bootgdt[NSEGS] = {
  [0] = SEGDESC(0, 0, 0),  // null
  [1] = SEGDESC(0, 0xfffff, SEG_R|SEG_CODE|SEG_S|SEG_DPL(0)|SEG_P|SEG_D|SEG_G),  // 32-bit kernel code
  [2] = SEGDESC(0, 0, SEG_R|SEG_CODE|SEG_S|SEG_DPL(0)|SEG_P|SEG_L|SEG_G),  // 64-bit kernel code
  [3] = SEGDESC(0, 0xfffff, SEG_W|SEG_S|SEG_DPL(0)|SEG_P|SEG_D|SEG_G),       // kernel data
  // The order of the user data and user code segments is
  // important for syscall instructions.  See initseg.
  [6] = SEGDESC(0, 0xfffff, SEG_W|SEG_S|SEG_DPL(3)|SEG_P|SEG_D|SEG_G),   // 64-bit user data
  [7] = SEGDESC(0, 0, SEG_R|SEG_CODE|SEG_S|SEG_DPL(3)|SEG_P|SEG_L|SEG_G),    // 64-bit user code
};


// Set up CPU's kernel segment descriptors.
// Run once on entry on each CPU.
void
seginit(void)
{
  struct cpu *c;
  struct desctr dtr;

  c = mycpu();
  memmove(c->gdt, bootgdt, sizeof bootgdt);
  dtr.limit = sizeof(c->gdt)-1;
  dtr.base = (uint64) c->gdt;
  lgdt((void *)&dtr.limit);

  // When executing a syscall instruction the CPU sets the SS selector
  // to (star >> 32) + 8 and the CS selector to (star >> 32).
  // When executing a sysret instruction the CPU sets the SS selector
  // to (star >> 48) + 8 and the CS selector to (star >> 48) + 16.
  uint64 star = ((((uint64)UCSEG|0x3)- 16)<<48)|((uint64)(KCSEG)<<32);
  writemsr(MSR_STAR, star);
  writemsr(MSR_LSTAR, (uint64)&sysentry);
  writemsr(MSR_SFMASK, FL_TF | FL_IF);

    // Initialize cpu-local storage.
  writegs(KDSEG);
  writemsr(MSR_GS_BASE, (uint64)c);
  writemsr(MSR_GS_KERNBASE, (uint64)c);
}

// Return the address of the PTE in page table pgdir
// that corresponds to virtual address va.  If alloc!=0,
// create any required page table pages.
static pte_t *
walkpgdir(pde_t *pml4, const void *va, int alloc)
{
  pde_t *pgtab = pml4;
  pde_t *pde;
  int level;
  
  for (level = L_PML4; level > 0; level--) {
    pde = &pgtab[PX(level, va)];
    if(*pde & PTE_P)
      pgtab = (pte_t*)P2V(PTE_ADDR(*pde));
    else {
      if(!alloc || (pgtab = (pte_t*)kalloc()) == 0)
        return 0;
      memset(pgtab, 0, PGSIZE);
      *pde = V2P(pgtab) | PTE_P | PTE_W | PTE_U;
    }
  }
  return &pgtab[PX(level, va)];
}

// Create PTEs for virtual addresses starting at va that refer to
// physical addresses starting at pa. va and size might not
// be page-aligned.
static int
mappages(pde_t *pgdir, void *va, uint64 size, uint64 pa, int perm)
{
  char *a, *last;
  pte_t *pte;

  a = (char*)PGROUNDDOWN((uint64)va);
  last = (char*)PGROUNDDOWN(((uint64)va) + size - 1);
  for(;;){
    if((pte = walkpgdir(pgdir, a, 1)) == 0)
      return -1;
    if(*pte & PTE_P)
      panic("remap");
    *pte = pa | perm | PTE_P;
    if(a == last)
      break;
    a += PGSIZE;
    pa += PGSIZE;
  }
  return 0;
}

// There is one page table per process, plus one that's used when
// a CPU is not running any process (kpml4). The kernel uses the
// current process's page table during system calls and interrupts;
// page protection bits prevent user code from using the kernel's
// mappings.
//
// setupkvm() and exec() set up every page table like this:
//
//   0..KERNBASE: user memory (text+data+stack+heap), mapped to
//                phys memory allocated by the kernel
//   KERNBASE..KERNBASE+EXTMEM: mapped to 0..EXTMEM (for I/O space)
//   KERNBASE+EXTMEM..data: mapped to EXTMEM..V2P(data)
//                for the kernel's instructions and r/o data
//   data..KERNBASE+PHYSTOP: mapped to V2P(data)..PHYSTOP,
//                                  rw data + free physical memory
//   0xfe000000..0: mapped direct (devices such as ioapic)
//
// The kernel allocates physical memory for its heap and for user memory
// between V2P(end) and the end of physical memory (PHYSTOP)
// (directly addressable from end..P2V(PHYSTOP)).

// This table defines the kernel's mappings, which are present in
// every process's page table.
static struct kmap {
  void *virt;
  uint64 phys_start;
  uint64 phys_end;
  int perm;
} kmap[] = {
 { (void*)KERNBASE, 0,             EXTMEM,    PTE_W}, // I/O space
 { (void*)KERNLINK, V2P(KERNLINK), V2P(data), 0},     // kern text+rodata
 { (void*)data,     V2P(data),     PHYSTOP,   PTE_W}, // kern data+memory
 { (void*)P2V(DEVSPACE), DEVSPACE, DEVSPACETOP, PTE_W}, // more devices
};

// Set up kernel part of a page table.
pde_t*
setupkvm(void)
{
  pde_t *pml4;
  struct kmap *k;

  if((pml4 = (pde_t*)kalloc()) == 0)
    return 0;
  memset(pml4, 0, PGSIZE);
  if (PHYSTOP > DEVSPACE)
    panic("PHYSTOP too high");
  for(k = kmap; k < &kmap[NELEM(kmap)]; k++) {
    if(mappages(pml4, k->virt, k->phys_end - k->phys_start,
                (uint)k->phys_start, k->perm) < 0) {
      freevm(pml4, 0);
      return 0;
    }
  }
  return pml4;
}

// Allocate one page table for the machine for the kernel address
// space for scheduler processes.
void
kvmalloc(void)
{
  kpml4 = setupkvm();
  switchkvm();
}

// Switch h/w page table register to the kernel-only page table,
// for when no process is running.
void
switchkvm(void)
{
  lcr3(V2P(kpml4));   // switch to the kernel page table
}


// Switch TSS and h/w page table to correspond to process p.
void
switchuvm(struct proc *p)
{
  struct desctr dtr;
  struct cpu *c;
  
  if(p == 0)
    panic("switchuvm: no process");
  if(p->kstack == 0)
    panic("switchuvm: no kstack");
  if(p->pgdir == 0)
    panic("switchuvm: no pgdir");

  pushcli();

  c = mycpu();
  uint64 base = (uint64) &(c->ts);
  c->gdt[TSSSEG>>3] =  SEGDESC(base, (sizeof(c->ts)-1), SEG_P|SEG_TSS64A);
  c->gdt[(TSSSEG>>3)+1] = SEGDESCHI(base);
  c->ts.rsp[0] = (uint64) p->kstack + KSTACKSIZE;
  c->ts.iomba = (ushort) 0xFFFF;

  dtr.limit = sizeof(c->gdt) - 1;
  dtr.base = (uint64)c->gdt;
  lgdt((void *)&dtr.limit);

  ltr(TSSSEG);

  lcr3(V2P(p->pgdir));  // switch to process's address space

  popcli();
}

// Load the initcode into address 0 of pgdir.
// sz must be less than a page.
void
inituvm(pde_t *pgdir, char *init, uint sz)
{
  char *mem;

  if(sz >= PGSIZE)
    panic("inituvm: more than a page");
  mem = kalloc();
  memset(mem, 0, PGSIZE);
  mappages(pgdir, 0, PGSIZE, V2P(mem), PTE_W|PTE_U);
  memmove(mem, init, sz);
}

// Load a program segment into pgdir.  addr must be page-aligned
// and the pages from addr to addr+sz must already be mapped.
int
loaduvm(pde_t *pgdir, char *addr, struct inode *ip, uint offset, uint sz)
{
  uint i, n;
  uint64 pa;
  pte_t *pte;

  if((uint64) addr % PGSIZE != 0)
    panic("loaduvm: addr must be page aligned");
  for(i = 0; i < sz; i += PGSIZE){
    if((pte = walkpgdir(pgdir, addr+i, 0)) == 0)
      panic("loaduvm: address should exist");
    pa = PTE_ADDR(*pte);
    if(sz - i < PGSIZE)
      n = sz - i;
    else
      n = PGSIZE;
    if(readi(ip, P2V(pa), offset+i, n) != n)
      return -1;
  }
  return 0;
}

// Allocate page tables and physical memory to grow process from oldsz to
// newsz, which need not be page aligned.  Returns new size or 0 on error.
int
allocuvm(pde_t *pgdir, uint oldsz, uint newsz)
{
  char *mem;
  uint64 a;

  if(newsz >= KERNBASE)
    return 0;
  if(newsz < oldsz)
    return oldsz;

  a = PGROUNDUP(oldsz);
  for(; a < newsz; a += PGSIZE){
    mem = kalloc();
    if(mem == 0){
      deallocuvm(pgdir, newsz, oldsz);
      return 0;
    }
    memset(mem, 0, PGSIZE);
    if(mappages(pgdir, (char*)a, PGSIZE, V2P(mem), PTE_W|PTE_U) < 0){
      deallocuvm(pgdir, newsz, oldsz);
      kfree(mem);
      return 0;
    }
  }
  return newsz;
}

// Deallocate user pages to bring the process size from oldsz to
// newsz.  oldsz and newsz need not be page-aligned, nor does newsz
// need to be less than oldsz.  oldsz can be larger than the actual
// process size.  Returns the new process size.
int
deallocuvm(pde_t *pml4, uint64 oldsz, uint64 newsz)
{
  pte_t *pte;
  uint64 a, pa;

  if(newsz >= oldsz)
    return oldsz;

  a = PGROUNDUP(newsz);
  for(; a  < oldsz; a += PGSIZE){
    pte = walkpgdir(pml4, (char*)a, 0);
    if(!pte)
      a = PGADDR(PDX(a) + 1, 0, 0) - PGSIZE;
    else if((*pte & PTE_P) != 0){
      pa = PTE_ADDR(*pte);
      if(pa == 0)
        panic("kfree");
      char *v = P2V(pa);
      kfree(v);
      *pte = 0;
    }
  }
  return newsz;
}

// Recursively free a page table
void
freelevel(pde_t *pgtab, int level) {
  int i;
  pde_t *pd;
  
  if (level > 0) {
    for(i = 0; i < NPDENTRIES; i++) {
      if(pgtab[i] & PTE_P){
        pd = (pdpe_t*)P2V(PTE_ADDR(pgtab[i]));
        freelevel(pd, level-1);
      }
    }
  }
  kfree((char*)pgtab);
}

// Free all the physical memory pages
// in the user part and page table
void
freevm(pde_t *pml4, uint64 sz)
{
  if(pml4 == 0)
    panic("freevm: no pgdir");

  deallocuvm(pml4, sz, 0);
  freelevel(pml4, L_PML4);
}

// Clear PTE_U on a page. Used to create an inaccessible
// page beneath the user stack.
void
clearpteu(pde_t *pgdir, char *uva)
{
  pte_t *pte;

  pte = walkpgdir(pgdir, uva, 0);
  if(pte == 0)
    panic("clearpteu");
  *pte &= ~PTE_U;
}

// Given a parent process's page table, create a copy
// of it for a child.
pde_t*
copyuvm(pde_t *pgdir, uint sz)
{
  pde_t *d;
  pte_t *pte;
  uint64 pa, i;
  uint flags;
  char *mem;

  if((d = setupkvm()) == 0)
    return 0;
  for(i = 0; i < sz; i += PGSIZE){
    if((pte = walkpgdir(pgdir, (void *) i, 0)) == 0)
      panic("copyuvm: pte should exist");
    if(!(*pte & PTE_P))
      panic("copyuvm: page not present");
    pa = PTE_ADDR(*pte);
    flags = PTE_FLAGS(*pte);
    if((mem = kalloc()) == 0)
      goto bad;
    memmove(mem, (char*)P2V(pa), PGSIZE);
    if(mappages(d, (void*)i, PGSIZE, V2P(mem), flags) < 0) {
      kfree(mem);
      goto bad;
    }
  }
  return d;

bad:
  freevm(d, sz);
  return 0;
}

//PAGEBREAK!
// Map user virtual address to kernel address.
char*
uva2ka(pde_t *pgdir, char *uva)
{
  pte_t *pte;

  pte = walkpgdir(pgdir, uva, 0);
  if((*pte & PTE_P) == 0)
    return 0;
  if((*pte & PTE_U) == 0)
    return 0;
  return (char*)P2V(PTE_ADDR(*pte));
}

// Copy len bytes from p to user address va in page table pgdir.
// Most useful when pgdir is not the current page table.
// uva2ka ensures this only works for PTE_U pages.
int
copyout(pde_t *pgdir, uint va, void *p, uint len)
{
  char *buf, *pa0;
  uint64 n, va0;

  buf = (char*)p;
  while(len > 0){
    va0 = (uint)PGROUNDDOWN(va);
    pa0 = uva2ka(pgdir, (char*)va0);
    if(pa0 == 0)
      return -1;
    n = PGSIZE - (va - va0);
    if(n > len)
      n = len;
    memmove(pa0 + (va - va0), buf, n);
    len -= n;
    buf += n;
    va = va0 + PGSIZE;
  }
  return 0;
}

//PAGEBREAK!
// Blank page.
//PAGEBREAK!
// Blank page.
//PAGEBREAK!
// Blank page.

