#include "param.h"
#include "types.h"
#include "defs.h"
#include "x86.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "elf.h"

extern char data[];  // defined by kernel.ld
pde_t *kpgdir;  // for use in scheduler()

__thread struct cpu *cpu;
__thread struct proc *proc;

static pde_t *kpml4;
static pde_t *kpdpt;

static void 
tss_set_rsp(uint *tss, uint n, uint64 rsp) {
  tss[n*2 + 1] = rsp;
  tss[n*2 + 2] = rsp >> 32;
}

void
syscallinit(void)
{
  wrmsr( MSR_STAR, ((uint64)USER_CS) << 48 | ((uint64)KERNEL_CS << 32));
  wrmsr( MSR_LSTAR, syscall_entry );
  wrmsr( MSR_CSTAR, ignore_sysret );
  
  
  wrmsr( MSR_SFMASK, FL_TF|FL_DF|FL_IF|FL_IOPL_3|FL_AC|FL_NT);
}


//extern void* vectors[];

// Set up CPU's kernel segment descriptors.
// Run once on entry on each CPU.
void
seginit(void)
{
  //uint64 *gdt;
  struct segdesc *gdt;
  uint *tss;
  uint64 addr;
  void *local;
  struct cpu *c;

  // create a page for cpu local storage 
  local = kalloc();
  memset(local, 0, PGSIZE);

  //gdt = (uint64*) local;
  gdt = (struct segdesc*) local;
  tss = (uint*) (((char*) local) + 1024);
  tss[16] = 0x00680000; // IO Map Base = End of TSS

  // point FS smack in the middle of our local storage page
  wrmsr(0xC0000100, ((uint64) local) + (PGSIZE / 2));

  c = &cpus[cpunum()];
  c->local = local;

  cpu = c;
  proc = 0;

  addr = (uint64) tss;
  gdt[0] =  (struct segdesc) {0};

  gdt[SEG_KCODE] = SEG((STA_X | STA_R),0, 0, APP_SEG, !DPL_USER, 1);

  gdt[SEG_UCODE] = SEG((STA_X | STA_R),0, 0, APP_SEG, DPL_USER, 1);

  gdt[SEG_KDATA] = SEG(STA_W,0, 0, APP_SEG, !DPL_USER, 0);

  gdt[SEG_KCPU]  = (struct segdesc) {0};

  gdt[SEG_UDATA] = SEG(STA_W,0, 0, APP_SEG, DPL_USER, 0);

  gdt[SEG_TSS+0] = SEG(STS_T64A, 0xb, addr, !APP_SEG, DPL_USER, 0);
  gdt[SEG_TSS+1] = SEG(0,addr >> 32, addr >> 48, 0, 0, 0);

//  struct gatedesc *gdtcg =(struct gatedesc*) &gdt[CALL_GATE];
//  SETCALLGATE(gdtcg,0,0,1);

  lgdt((void*) gdt, 8 * sizeof(struct segdesc));

  ltr(SEG_TSS << 3);
};

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


pde_t*
setupkvm(void)
{
  pde_t *pml4 = (pde_t*) kalloc();
  memset(pml4, 0, PGSIZE);
  pml4[256] = v2p(kpdpt) | PTE_P | PTE_W;
  return pml4;
};

// Allocate one page table for the machine for the kernel address
// space for scheduler processes.
//
// linear map the first 4GB of physical memory starting
// at 0xFFFF800000000000
void
kvmalloc(void)
{
  int n;
  kpml4 = (pde_t*) kalloc();
  memset(kpml4, 0, PGSIZE);

  // the kernel memory region starts at KERNBASE and up
  // allocate one PDPT at the bottom of that range.
  kpdpt = (pde_t*) kalloc();
  memset(kpdpt, 0, PGSIZE);
  kpml4[PMX(KERNBASE)] = v2p(kpdpt) | PTE_P | PTE_W;

  // at the bottom of the PDPT, one huge 1GB mapping using the PS bit
  kpdpt[0] = 0 | PTE_PS | PTE_P | PTE_W;
  kpdpt[1] = 1<<20 | PTE_PS | PTE_P | PTE_W;
  kpdpt[2] = 2*(1<<20) | PTE_PS | PTE_P | PTE_W;
  kpdpt[3] = 3*(1<<20) | PTE_PS | PTE_P | PTE_W;  

  /*
  // TODO: why is this 509?
  iopgdir = (pde_t*) kalloc();
  memset(iopgdir, 0, PGSIZE);
  kpdpt[509] = v2p(iopgdir) | PTE_P | PTE_W;
  
  for (n = 0; n < 16; n++)
    iopgdir[n] = (DEVSPACE + (n << PDXSHIFT)) |   \
      PTE_PS | PTE_P | PTE_W | PTE_PWT | PTE_PCD;
  */
  switchkvm();
}

void
switchkvm(void)
{
  lcr3(v2p(kpml4));
}

void
switchuvm(struct proc *p)
{
  uint *tss;
  pushcli();
  if(p->pgdir == 0)
    panic("switchuvm: no pgdir");
  tss = (uint*) (((char*) cpu->local) + 1024);
  tss_set_rsp(tss, 0, (addr_t)proc->kstack + KSTACKSIZE);
  lcr3(v2p(p->pgdir));
  popcli();

}

// Return the address of the PTE in page table pgdir
// that corresponds to virtual address va.  If alloc!=0,
// create any required page table pages.
//
// In 64-bit mode, the page table has four levels: PML4, PDPT, PD and PT
// For each level, we dereference the correct entry, or allocate and
// initialize entry if the PTE_P bit is not set

static pte_t *
walkpgdir(pde_t *pml4, const void *va, int alloc)
{
  pml4e_t *pml4e;
  pdpe_t *pdp;
  pdpe_t *pdpe;
  pde_t *pde;
  pde_t *pd;
  pte_t *pgtab;
  

  // from the PML4, find or allocate the appropriate PDP table
  pml4e = &pml4[PMX(va)];
  if(*pml4e & PTE_P)
    pdp = (pdpe_t*)P2V(PTE_ADDR(*pml4e));  
  else {
    if(!alloc || (pdp = (pdpe_t*)kalloc()) == 0)
      return 0;
    memset(pdp, 0, PGSIZE);
    *pml4e = V2P(pdp) | PTE_P | PTE_W | PTE_U;
  }

  //from the PDP, find or allocate the appropriate PD (page directory)
  pdpe = &pdp[PDPX(va)];  
  if(*pdpe & PTE_P) 
    pd = (pde_t*)P2V(PTE_ADDR(*pdpe));
  else {
    if(!alloc || (pd = (pde_t*)kalloc()) == 0)//allocate page table
      return 0;
    memset(pd, 0, PGSIZE);
    *pdpe = V2P(pd) | PTE_P | PTE_W | PTE_U;
  }

  // from the PD, find or allocate the appropriate page table 
  pde = &pd[PDX(va)]; 
  if(*pde & PTE_P)
    pgtab = (pte_t*)P2V(PTE_ADDR(*pde));
  else {
    if(!alloc || (pgtab = (pte_t*)kalloc()) == 0)//allocate page table
      return 0;
    memset(pgtab, 0, PGSIZE);
    *pde = V2P(pgtab) | PTE_P | PTE_W | PTE_U;
  }
  
  return &pgtab[PTX(va)];
}


// Create PTEs for virtual addresses starting at va that refer to
// physical addresses starting at pa. va and size might not
// be page-aligned.
static int
mappages(pde_t *pgdir, void *va, addr_t size, addr_t pa, int perm)
{
  char *a, *last;
  pte_t *pte;

  a = (char*)PGROUNDDOWN((addr_t)va);
  last = (char*)PGROUNDDOWN(((addr_t)va) + size - 1);
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
  addr_t pa;
  pte_t *pte;

  if((addr_t) addr % PGSIZE != 0)
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
  addr_t a;

  if(newsz >= KERNBASE)
    return 0;
  if(newsz < oldsz)
    return oldsz;

  a = PGROUNDUP(oldsz);
  for(; a < newsz; a += PGSIZE){
    mem = kalloc();
    if(mem == 0){
      cprintf("allocuvm out of memory\n");
      deallocuvm(pgdir, newsz, oldsz);
      return 0;
    }
    memset(mem, 0, PGSIZE);
    if(mappages(pgdir, (char*)a, PGSIZE, V2P(mem), PTE_W|PTE_U) < 0){
      cprintf("allocuvm out of memory (2)\n");
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
deallocuvm(pde_t *pgdir, uint64 oldsz, uint64 newsz)
{
  pte_t *pte;
  addr_t a, pa;

  if(newsz >= oldsz)
    return oldsz;

  a = PGROUNDUP(newsz);
  for(; a  < oldsz; a += PGSIZE){
    pte = walkpgdir(pgdir, (char*)a, 0);
    if(pte && (*pte & PTE_P) != 0){
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

// Free all the pages mapped by, and all the memory used for,
// this page table
void
freevm(pde_t *pml4)
{
  uint i, j, k;
  pde_t *pdp, *pd;

  if(pml4 == 0)
    panic("freevm: no pgdir");
  
  deallocuvm(pml4, 0x3fa00000, 0);
  
  // then need to loop through pml4 entry
  for(i = 0; i < (NPDENTRIES/2); i++){
    if(pml4[i] & PTE_P){
      pdp = (pdpe_t*)P2V(PTE_ADDR(pml4[i])); 

      // and every entry in the corresponding pdpt
      for(j = 0; j < NPDENTRIES; j++){
        if(pdp[j] & PTE_P){ 
          pd = (pde_t*)P2V(PTE_ADDR(pdp[j]));

          // and every entry in the corresponding page directory
          for(k = 0; k < (NPDENTRIES); k++){
            if(pd[k] & PTE_P) {
              char * v = P2V(PTE_ADDR(pd[k]));
              // freeing every page table
              kfree((char*)v);
            }
          }
          // freeing every page directory
          kfree((char*)pd);
        }
      }
      // freeing every page directory pointer table
      kfree((char*)pdp);
    }
  }
  // freeing the pml4
  kfree((char*)pml4);
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
  addr_t pa, i, flags;
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
    if(mappages(d, (void*)i, PGSIZE, V2P(mem), flags) < 0)
      goto bad;
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
  addr_t n, va0;

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



