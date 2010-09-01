#include "param.h"
#include "types.h"
#include "defs.h"
#include "x86.h"
#include "mmu.h"
#include "proc.h"
#include "elf.h"

// The mappings from logical to linear are one to one (i.e.,
// segmentation doesn't do anything).
// There is one page table per process, plus one that's used
// when a CPU is not running any process (kpgdir).
// A user process uses the same page table as the kernel; the
// page protection bits prevent it from using anything other
// than its memory.
// 
// setupkvm() and exec() set up every page table like this:
//   0..640K          : user memory (text, data, stack, heap)
//   640K..1M         : mapped direct (for IO space)
//   1M..end          : mapped direct (for the kernel's text and data)
//   end..PHYSTOP     : mapped direct (kernel heap and user pages)
//   0xfe000000..0    : mapped direct (devices such as ioapic)
//
// The kernel allocates memory for its heap and for user memory
// between kernend and the end of physical memory (PHYSTOP).
// The virtual address space of each user program includes the kernel
// (which is inaccessible in user mode).  The user program addresses
// range from 0 till 640KB (USERTOP), which where the I/O hole starts
// (both in physical memory and in the kernel's virtual address
// space).

#define USERTOP  0xA0000

static pde_t *kpgdir;  // for use in scheduler()

// return the address of the PTE in page table pgdir
// that corresponds to linear address va.  if create!=0,
// create any required page table pages.
static pte_t *
walkpgdir(pde_t *pgdir, const void *va, int create)
{
  uint r;
  pde_t *pde;
  pte_t *pgtab;

  pde = &pgdir[PDX(va)];
  if(*pde & PTE_P){
    pgtab = (pte_t*) PTE_ADDR(*pde);
  } else if(!create || !(r = (uint) kalloc()))
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

// create PTEs for linear addresses starting at la that refer to
// physical addresses starting at pa. la and size might not
// be page-aligned.
static int
mappages(pde_t *pgdir, void *la, uint size, uint pa, int perm)
{
  char *first = PGROUNDDOWN(la);
  char *last = PGROUNDDOWN(la + size - 1);
  char *a = first;
  while(1){
    pte_t *pte = walkpgdir(pgdir, a, 1);
    if(pte == 0)
      return 0;
    if(*pte & PTE_P)
      panic("remap");
    *pte = pa | perm | PTE_P;
    if(a == last)
      break;
    a += PGSIZE;
    pa += PGSIZE;
  }
  return 1;
}

// Set up CPU's kernel segment descriptors.
// Run once at boot time on each CPU.
void
ksegment(void)
{
  struct cpu *c;

  // Map virtual addresses to linear addresses using identity map.
  // Cannot share a CODE descriptor for both kernel and user
  // because it would have to have DPL_USR, but the CPU forbids
  // an interrupt from CPL=0 to DPL=3.
  c = &cpus[cpunum()];
  c->gdt[SEG_KCODE] = SEG(STA_X|STA_R, 0, 0xffffffff, 0);
  c->gdt[SEG_KDATA] = SEG(STA_W, 0, 0xffffffff, 0);
  c->gdt[SEG_UCODE] = SEG(STA_X|STA_R, 0, 0xffffffff, DPL_USER);
  c->gdt[SEG_UDATA] = SEG(STA_W, 0, 0xffffffff, DPL_USER);

  // map cpu, and curproc
  c->gdt[SEG_KCPU] = SEG(STA_W, &c->cpu, 8, 0);

  lgdt(c->gdt, sizeof(c->gdt));
  loadgs(SEG_KCPU << 3);
  
  // Initialize cpu-local storage.
  cpu = c;
  proc = 0;
}

// Switch h/w page table and TSS registers to point to process p.
void
switchuvm(struct proc *p)
{
  pushcli();

  // Setup TSS
  cpu->gdt[SEG_TSS] = SEG16(STS_T32A, &cpu->ts, sizeof(cpu->ts)-1, 0);
  cpu->gdt[SEG_TSS].s = 0;
  cpu->ts.ss0 = SEG_KDATA << 3;
  cpu->ts.esp0 = (uint)proc->kstack + KSTACKSIZE;
  ltr(SEG_TSS << 3);

  if(p->pgdir == 0)
    panic("switchuvm: no pgdir\n");

  lcr3(PADDR(p->pgdir));  // switch to new address space
  popcli();
}

// Switch h/w page table register to the kernel-only page table, for when
// no process is running.
void
switchkvm()
{
  lcr3(PADDR(kpgdir));   // Switch to the kernel page table
}

// Set up kernel part of a page table.
pde_t*
setupkvm(void)
{
  pde_t *pgdir;

  // Allocate page directory
  if(!(pgdir = (pde_t *) kalloc()))
    return 0;
  memset(pgdir, 0, PGSIZE);
  // Map IO space from 640K to 1Mbyte
  if(!mappages(pgdir, (void *)USERTOP, 0x60000, USERTOP, PTE_W))
    return 0;
  // Map kernel and free memory pool
  if(!mappages(pgdir, (void *)0x100000, PHYSTOP-0x100000, 0x100000, PTE_W))
    return 0;
  // Map devices such as ioapic, lapic, ...
  if(!mappages(pgdir, (void *)0xFE000000, 0x2000000, 0xFE000000, PTE_W))
    return 0;
  return pgdir;
}

// return the physical address that a given user address
// maps to. the result is also a kernel logical address,
// since the kernel maps the physical memory allocated to user
// processes directly.
char*
uva2ka(pde_t *pgdir, char *uva)
{    
  pte_t *pte = walkpgdir(pgdir, uva, 0);
  if(pte == 0) return 0;
  uint pa = PTE_ADDR(*pte);
  return (char *)pa;
}

// allocate sz bytes more memory for a process starting at the
// given user address; allocates physical memory and page
// table entries. addr and sz need not be page-aligned.
// it is a no-op for any parts of the requested memory
// that are already allocated.
int
allocuvm(pde_t *pgdir, char *addr, uint sz)
{
  if(addr + sz > (char*)USERTOP)
    return 0;
  char *first = PGROUNDDOWN(addr);
  char *last = PGROUNDDOWN(addr + sz - 1);
  char *a;
  for(a = first; a <= last; a += PGSIZE){
    pte_t *pte = walkpgdir(pgdir, a, 0);
    if(pte == 0 || (*pte & PTE_P) == 0){
      char *mem = kalloc();
      if(mem == 0){
        cprintf("allocuvm out of memory\n");
        deallocuvm(pgdir, addr, sz);
        return 0;
      }
      memset(mem, 0, PGSIZE);
      mappages(pgdir, a, PGSIZE, PADDR(mem), PTE_W|PTE_U);
    }
  }
  return 1;
}

// deallocate some of the user pages, in response to sbrk()
// with a negative argument. if addr is not page-aligned,
// then only deallocates starting at the next page boundary.
int
deallocuvm(pde_t *pgdir, char *addr, uint sz)
{
  if(addr + sz > (char*)USERTOP)
    return 0;
  char *first = (char*) PGROUNDUP((uint)addr);
  char *last = PGROUNDDOWN(addr + sz - 1);
  char *a;
  for(a = first; a <= last; a += PGSIZE){
    pte_t *pte = walkpgdir(pgdir, a, 0);
    if(pte && (*pte & PTE_P) != 0){
      uint pa = PTE_ADDR(*pte);
      if(pa == 0)
        panic("deallocuvm");
      kfree((void *) pa);
      *pte = 0;
    }
  }
  return 1;
}

// free a page table and all the physical memory pages
// in the user part.
void
freevm(pde_t *pgdir)
{
  uint i, j, da;

  if(!pgdir)
    panic("freevm: no pgdir\n");
  for(i = 0; i < NPDENTRIES; i++){
    da = PTE_ADDR(pgdir[i]);
    if(da != 0){
      pte_t *pgtab = (pte_t*) da;
      for(j = 0; j < NPTENTRIES; j++){
        if(pgtab[j] != 0){
          uint pa = PTE_ADDR(pgtab[j]);
          uint va = PGADDR(i, j, 0);
          if(va < USERTOP)   // user memory
            kfree((void *) pa);
          pgtab[j] = 0;
        }
      }
      kfree((void *) da);
      pgdir[i] = 0;
    }
  }
  kfree((void *) pgdir);
}

int
loaduvm(pde_t *pgdir, char *addr, struct inode *ip, uint offset, uint sz)
{
  uint i, pa, n;
  pte_t *pte;

  if((uint)addr % PGSIZE != 0)
    panic("loaduvm: addr must be page aligned\n");
  for(i = 0; i < sz; i += PGSIZE){
    if(!(pte = walkpgdir(pgdir, addr+i, 0)))
      panic("loaduvm: address should exist\n");
    pa = PTE_ADDR(*pte);
    if(sz - i < PGSIZE) n = sz - i;
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

  for(i = 0; i < sz; i += PGSIZE){
    if(!(pte = walkpgdir(pgdir, (void *)(i+addr), 0)))
      panic("inituvm: pte should exist\n");
    off = (i+(uint)addr) % PGSIZE;
    pa = PTE_ADDR(*pte);
    if(sz - i < PGSIZE) n = sz - i;
    else n = PGSIZE;
    memmove((char *)pa+off, init+i, n);
  }
}

// given a parent process's page table, create a copy
// of it for a child.
pde_t*
copyuvm(pde_t *pgdir, uint sz)
{
  pde_t *d = setupkvm();
  pte_t *pte;
  uint pa, i;
  char *mem;

  if(!d) return 0;
  for(i = 0; i < sz; i += PGSIZE){
    if(!(pte = walkpgdir(pgdir, (void *)i, 0)))
      panic("copyuvm: pte should exist\n");
    if(!(*pte & PTE_P))
      panic("copyuvm: page not present\n");
    pa = PTE_ADDR(*pte);
    if(!(mem = kalloc()))
      goto bad;
    memmove(mem, (char *)pa, PGSIZE);
    if(!mappages(d, (void *)i, PGSIZE, PADDR(mem), PTE_W|PTE_U))
      goto bad;
  }
  return d;

bad:
  freevm(d);
  return 0;
}

// Allocate one page table for the machine for the kernel address
// space for scheduler processes.
void
kvmalloc(void)
{
  kpgdir = setupkvm();
}

// Turn on paging.
void
vmenable(void)
{
  uint cr0;

  switchkvm(); // load kpgdir into cr3
  cr0 = rcr0();
  cr0 |= CR0_PG;
  lcr0(cr0);
}

