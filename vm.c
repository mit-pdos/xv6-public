#include "param.h"
#include "types.h"
#include "defs.h"
#include "x86.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "elf.h"
#include "swap.h"
#include "rammap.h"

extern char data[];  // defined by kernel.ld
pde_t *kpgdir;  // for use in scheduler()

// Set up CPU's kernel segment descriptors.
// Run once on entry on each CPU.
void
seginit(void)
{
  struct cpu *c;

  // Map "logical" addresses to virtual addresses using identity map.
  // Cannot share a CODE descriptor for both kernel and user
  // because it would have to have DPL_USR, but the CPU forbids
  // an interrupt from CPL=0 to DPL=3.
  c = &cpus[cpuid()];
  c->gdt[SEG_KCODE] = SEG(STA_X|STA_R, 0, 0xffffffff, 0);
  c->gdt[SEG_KDATA] = SEG(STA_W, 0, 0xffffffff, 0);
  c->gdt[SEG_UCODE] = SEG(STA_X|STA_R, 0, 0xffffffff, DPL_USER);
  c->gdt[SEG_UDATA] = SEG(STA_W, 0, 0xffffffff, DPL_USER);
  lgdt(c->gdt, sizeof(c->gdt));
}

// Return the address of the PTE in page table pgdir
// that corresponds to virtual address va.  If alloc!=0,
// create any required page table pages.
pte_t *
walkpgdir(pde_t *pgdir, const void *va, int alloc)
{
  pde_t *pde;
  pte_t *pgtab;

  pde = &pgdir[PDX(va)];
  if(*pde & PTE_P){
    pgtab = (pte_t*)P2V(PTE_ADDR(*pde));
  } else {
    if(!alloc || (pgtab = (pte_t*)kalloc()) == 0)
      return 0;
    // Make sure all those PTE_P bits are zero.
    memset(pgtab, 0, PGSIZE);
    // The permissions here are overly generous, but they can
    // be further restricted by the permissions in the page table
    // entries, if necessary.
    *pde = V2P(pgtab) | PTE_P | PTE_W | PTE_U;
  }
  return &pgtab[PTX(va)];
}

// Create PTEs for virtual addresses starting at va that refer to
// physical addresses starting at pa. va and size might not
// be page-aligned.
int
mappages(pde_t *pgdir, void *va, uint size, uint pa, int perm, uint pid)
{
  char *a, *last;
  pte_t *pte;

  a = (char*)PGROUNDDOWN((uint)va);
  last = (char*)PGROUNDDOWN(((uint)a) + size - 1);

  for(;;){
    if((pte = walkpgdir(pgdir, a, 1)) == 0)
      return -1;
    if(*pte & PTE_P)
      panic("mappages: remap");
    // Don't prematurely mark pages as available in PTE.
    *pte = pa | perm;

    // If the page to be mapped has user permissions enabled
    // It is a page which will be accessed by user processes
    // hence, it should be swappable.
    if(perm & PTE_U)
      rammap_make_entry(pa, (uint)a, pid, 1);

    if(a == last)
      break;
    a += PGSIZE;
    pa += PGSIZE;
  }
  return 0;
}

// There is one page table per process, plus one that's used when
// a CPU is not running any process (kpgdir). The kernel uses the
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
  uint phys_start;
  uint phys_end;
  int perm;
} kmap[] = {
 { (void*)KERNBASE, 0,             EXTMEM,    PTE_P|PTE_W}, // I/O space
 { (void*)KERNLINK, V2P(KERNLINK), V2P(data), PTE_P},     // kern text+rodata
 { (void*)data,     V2P(data),     PHYSTOP,   PTE_P|PTE_W}, // kern data+memory
 { (void*)DEVSPACE, DEVSPACE,      0,         PTE_P|PTE_W}, // more devices
};

// Set up kernel part of a page table.
pde_t*
setupkvm(void)
{
  pde_t *pgdir;
  struct kmap *k;

  if((pgdir = (pde_t*)kalloc()) == 0)
    return 0;
  memset(pgdir, 0, PGSIZE);
  if (P2V(PHYSTOP) > (void*)DEVSPACE)
    panic("PHYSTOP too high");
  for(k = kmap; k < &kmap[NELEM(kmap)]; k++)
    if(mappages(pgdir, k->virt, k->phys_end - k->phys_start,
                (uint)k->phys_start, k->perm, PID_KERNEL) < 0) {
      freevm(pgdir);
      return 0;
    }
  return pgdir;
}

// Allocate one page table for the machine for the kernel address
// space for scheduler processes.
void
kvmalloc(void)
{
  kpgdir = setupkvm();
  switchkvm();
}

// Switch h/w page table register to the kernel-only page table,
// for when no process is running.
void
switchkvm(void)
{
  lcr3(V2P(kpgdir));   // switch to the kernel page table
}

// Switch TSS and h/w page table to correspond to process p.
void
switchuvm(struct proc *p)
{
  if(p == 0)
    panic("switchuvm: no process");
  if(p->kstack == 0)
    panic("switchuvm: no kstack");
  if(p->pgdir == 0)
    panic("switchuvm: no pgdir");

  pushcli();
  mycpu()->gdt[SEG_TSS] = SEG16(STS_T32A, &mycpu()->ts,
                                sizeof(mycpu()->ts)-1, 0);
  mycpu()->gdt[SEG_TSS].s = 0;
  mycpu()->ts.ss0 = SEG_KDATA << 3;
  mycpu()->ts.esp0 = (uint)p->kstack + KSTACKSIZE;
  // setting IOPL=0 in eflags *and* iomb beyond the tss segment limit
  // forbids I/O instructions (e.g., inb and outb) from user space
  mycpu()->ts.iomb = (ushort) 0xFFFF;
  ltr(SEG_TSS << 3);
  lcr3(V2P(p->pgdir));  // switch to process's address space
  popcli();
}

// Load the initcode into address 0 of pgdir.
// sz must be less than a page.
/*excuse initcode from demand paging*/
void
inituvm(pde_t *pgdir, char *init, uint sz, uint pid)
{
  char *mem;

  if(sz >= PGSIZE)
    panic("inituvm: more than a page");
  mem = kalloc();
  memset(mem, 0, PGSIZE);
  mappages(pgdir, 0, PGSIZE, V2P(mem), PTE_P|PTE_W|PTE_U, pid);
  memmove(mem, init, sz);
}

// Allocate a new page to store the info
// about loadable segments in the ELF file.
static struct elfprof*
allocprof(void)
{
  char *ep;
  if((ep = kalloc()) == 0)
    return 0;
  memset(ep, 0, PGSIZE);
  return (struct elfprof *)ep;
}

// Record all the info about loadable segments needed
// by the loader to dynamically load content from disk
// upon a page fault. (to be called by exec)
struct elfprof*
recorduvm(struct elfprof *ep, uint vaddr, uint off, uint filesz, uint memsz)
{
  if(!ep)
    if((ep = allocprof()) == 0)
      return 0;

  if(ep->numseg == MAXSEG){
    struct elfprof *nep;
    if((nep = allocprof()) == 0){
      freeprof(ep);
      return 0;
    }
    nep->next = ep;
    ep = nep;
  }

  if(!ep->numseg)
    ep->start_vaddr = vaddr;
  ep->end_vaddr = vaddr + memsz;

  ep->ls[ep->numseg].vaddr = vaddr;
  ep->ls[ep->numseg].off = off;
  ep->ls[ep->numseg].filesz = filesz;
  ep->ls[ep->numseg].memsz = memsz;
  ep->numseg++;
  return ep;
}

// Recursively copies the ELF profile of one process
// To be called during fork.
struct elfprof*
copyprof(struct elfprof *ep)
{
  if(!ep)
    return 0;
  struct elfprof *nep;
  if((nep = allocprof()) == 0)
    return 0;
  nep->numseg = ep->numseg;
  nep->start_vaddr = ep->start_vaddr;
  nep->end_vaddr = ep->end_vaddr;
  nep->next = copyprof(ep->next);
  if(ep->next && !nep->next){
    kfree((char *)nep);
    return 0;
  }
  memmove((char *)nep->ls, (char *)ep->ls, MAXSEG * sizeof(struct loadseg));
  return nep;
}

// Recursively frees the pages allocated for ELF profile
void
freeprof(struct elfprof *ep)
{
  if(ep == 0)
    return;
  freeprof(ep->next);
  kfree((char *)ep);
}

// Deallocate user pages to bring the process size from oldsz to
// newsz.  oldsz and newsz need not be page-aligned, nor does newsz
// need to be less than oldsz.  oldsz can be larger than the actual
// process size.  Returns the new process size.
/* Updated deallocuvm() to skip over PTEs with absent PTE_P*/
int
deallocuvm(pde_t *pgdir, uint oldsz, uint newsz)
{
  pte_t *pte;
  uint a, pa;

  if(newsz >= oldsz)
    return oldsz;

  a = PGROUNDUP(newsz);
  for(; a  < oldsz; a += PGSIZE){
    pte = walkpgdir(pgdir, (char*)a, 0);

    // If page table is not allocated for this PDE
    /* Since we are implementing pure demand paging
      where even the page tables may not be allocated
      beforehand, so walkpgdir() can't find the PTE
      for such cases and return 0.*/
    // Just skip to the next PDE
    if(!pte){
      a = PGADDR(PDX(a) + 1, 0, 0) - PGSIZE;
      continue;
    }

    // If page is not allocated for this PTE
    // Nothing to do, just continue
    else if(!*pte)
      continue;

    // If page is swapped out and currently exists
    // in the swapspace.
    // Just decrement the swapslot refcount.
    else if(!(*pte & PTE_P))
      swap_decrease_refcount(GET_SWAPSLOT((uint)*pte));

    // If page is allocated for this PTE and present
    // in the RAM
    else{
      pa = PTE_ADDR(*pte);
      if(pa == 0)
        panic("kfree");
      char *v = P2V(pa);
      kfree(v);
    }
    *pte = 0;
  }
  return newsz;
}

// Free a page table and all the physical memory pages
// in the user part.
void
freevm(pde_t *pgdir)
{
  uint i;

  if(pgdir == 0)
    panic("freevm: no pgdir");
  deallocuvm(pgdir, KERNBASE, 0);
  for(i = 0; i < NPDENTRIES; i++){
    if(pgdir[i] & PTE_P){
      char * v = P2V(PTE_ADDR(pgdir[i]));
      kfree(v);
    }
  }
  kfree((char*)pgdir);
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
  rammap_make_entry((uint)*pte, 0, PID_KERNEL, 0);
}

// Given a parent process's page table, create a copy
// of it for a child.
pde_t*
copyuvm(pde_t *pgdir, uint sz, uint child_pid)
{
  pde_t *d;
  pte_t *pte, *new_pte;
  uint pa, i, flags;
  char *mem;

  if((d = setupkvm()) == 0)
    return 0;
  for(i = 0; i < sz; i += PGSIZE){
    /* Here walkpgdir() would fail if PTE couldn't be
      found for given address.*/
    // If no page table allocated for current PDE.
    if((pte = walkpgdir(pgdir, (void *) i, 0)) == 0)
      i = PGADDR(PDX(i) + 1, 0, 0) - PGSIZE;

    // If page is not allocated for this PTE
    // Nothing to copy, continue
    else if(!*pte)
      continue;

    // If the parent's page is currently swapped out.
    else if(!(*pte & PTE_P)) {
      // Get the slot number in swapspace from the PTE.
      uint slot = GET_SWAPSLOT((uint)*pte);

      if(!swap_increase_refcount(slot))
        goto bad;

      // Copy the swapslot index to the corresponding PTE
      // of the child
      /* Failure of walkpgdir() here usually means out of
        memory, so abort copyuvm() and call freevm()*/
      if((new_pte = walkpgdir(d, (char *)i, 1)) == 0) {
        // revert the refcount increment done before
        swap_decrease_refcount(slot);
        goto bad;
      }

      // child PTE points to the same swapslot
      *new_pte = *pte;
    }

    // If a physical page is allocated for the parent,
    // allocate a physical page for the child and copy
    // its contents. (no C.O.W in xv6)
    else {
      pa = PTE_ADDR(*pte);
      flags = PTE_FLAGS(*pte);
      rammap_make_entry(pa, i, myproc()->pid, 0);
      if((mem = kalloc()) == 0) {
        rammap_make_entry(pa, i, myproc()->pid, 1);
        goto bad;
      }
      memmove(mem, (char*)P2V((char *)pa), PGSIZE);
      rammap_make_entry(pa, i, myproc()->pid, 1);
      if(mappages(d, (void*)i, PGSIZE, V2P((uint)mem), flags, child_pid) < 0) {
        kfree((char *)mem);
        goto bad;
      }
    }
  }

  return d;

bad:
  freevm(d);
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
  uint n, va0;

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
