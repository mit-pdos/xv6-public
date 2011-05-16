#include "param.h"
#include "types.h"
#include "defs.h"
#include "x86.h"
#include "mmu.h"
#include "spinlock.h"
#include "condvar.h"
#include "queue.h"
#include "proc.h"
#include "elf.h"
#include "kalloc.h"

extern char data[];  // defined in data.S

static pde_t *kpgdir;  // for use in scheduler()

// Allocate one page table for the machine for the kernel address
// space for scheduler processes.
void
kvmalloc(void)
{
  kpgdir = setupkvm();
}

// Set up CPU's kernel segment descriptors.
// Run once at boot time on each CPU.
void
seginit(void)
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

  // Map cpu, curproc, ptable, kmem, runq
  c->gdt[SEG_KCPU] = SEG(STA_W, &c->cpu, 20, 0);

  lgdt(c->gdt, sizeof(c->gdt));
  loadgs(SEG_KCPU << 3);
  
  // Initialize cpu-local storage.
  cpu = c;
  proc = 0;
  ptable = &ptables[cpunum()];
  kmem = &kmems[cpunum()];
  runq = &runqs[cpunum()];
}

// Return the address of the PTE in page table pgdir
// that corresponds to linear address va.  If create!=0,
// create any required page table pages.
static pte_t *
walkpgdir(pde_t *pgdir, const void *va, int create)
{
  pde_t *pde;
  pte_t *pgtab;

  pde = &pgdir[PDX(va)];
  if(*pde & PTE_P){
    pgtab = (pte_t*)PTE_ADDR(*pde);
  } else {
    if(!create || (pgtab = (pte_t*)kalloc()) == 0)
      return 0;
    // Make sure all those PTE_P bits are zero.
    memset(pgtab, 0, PGSIZE);
    // The permissions here are overly generous, but they can
    // be further restricted by the permissions in the page table 
    // entries, if necessary.
    *pde = PADDR(pgtab) | PTE_P | PTE_W | PTE_U;
  }
  return &pgtab[PTX(va)];
}

// Create PTEs for linear addresses starting at la that refer to
// physical addresses starting at pa. la and size might not
// be page-aligned.
static int
mappages(pde_t *pgdir, void *la, uint size, uint pa, int perm)
{
  char *a, *last;
  pte_t *pte;
  
  a = PGROUNDDOWN(la);
  last = PGROUNDDOWN(la + size - 1);
  for(;;){
    pte = walkpgdir(pgdir, a, 1);
    if(pte == 0)
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

static int
updatepages(pde_t *pgdir, void *begin, void *end, int perm)
{
  char *a, *last;
  pte_t *pte;

  a = PGROUNDDOWN(begin);
  last = PGROUNDDOWN(end);
  for (;;) {
    pte = walkpgdir(pgdir, a, 1);
    if(pte != 0)
      *pte = PTE_ADDR(*pte) | perm | PTE_P;
    if (a == last)
      break;
    a += PGSIZE;
  }
  return 0;
}

static int
clearpages(pde_t *pgdir, void *begin, void *end)
{
  char *a, *last;
  pte_t *pte;

  a = PGROUNDDOWN(begin);
  last = PGROUNDDOWN(end);
  for (;;) {
    pte = walkpgdir(pgdir, a, 1);
    if(pte != 0)
      *pte = 0;
    if (a == last)
      break;
    a += PGSIZE;
  }
  return 0;
}

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
static struct kmap {
  void *p;
  void *e;
  int perm;
} kmap[] = {
  {(void*)USERTOP,    (void*)0x100000, PTE_W},  // I/O space
  {(void*)0x100000,   data,            0    },  // kernel text, rodata
  {data,              (void*)PHYSTOP,  PTE_W},  // kernel data, memory
  {(void*)0xFE000000, 0,               PTE_W},  // device mappings
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
  k = kmap;
  for(k = kmap; k < &kmap[NELEM(kmap)]; k++)
    if(mappages(pgdir, k->p, k->e - k->p, (uint)k->p, k->perm) < 0)
      return 0;

  return pgdir;
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

// Switch h/w page table register to the kernel-only page table,
// for when no process is running.
void
switchkvm(void)
{
  lcr3(PADDR(kpgdir));   // switch to the kernel page table
}

// Switch TSS and h/w page table to correspond to process p.
void
switchuvm(struct proc *p)
{
  pushcli();
  cpu->gdt[SEG_TSS] = SEG16(STS_T32A, &cpu->ts, sizeof(cpu->ts)-1, 0);
  cpu->gdt[SEG_TSS].s = 0;
  cpu->ts.ss0 = SEG_KDATA << 3;
  cpu->ts.esp0 = (uint)proc->kstack + KSTACKSIZE;
  ltr(SEG_TSS << 3);
  if(p->pgdir == 0)
    panic("switchuvm: no pgdir");
  lcr3(PADDR(p->pgdir));  // switch to new address space
  popcli();
}

struct {
  struct vmnode n[1024];
} vmnodes;

struct {
  struct vmap m[128];
} vmaps;

struct vmnode *
vmn_alloc(uint npg, uint type)
{
  for(uint i = 0; i < sizeof(vmnodes.n) / sizeof(vmnodes.n[0]); i++) {
    struct vmnode *n = &vmnodes.n[i];
    if(n->alloc == 0 && __sync_bool_compare_and_swap(&n->alloc, 0, 1)) {
      if(npg > sizeof(n->page) / sizeof(n->page[0])) {
	panic("vmnode too big\n");
      }
      n->npages = npg;
      n->ref = 0;
      n->ip = 0;
      n->type = type;
      return n;
    }
  }
  panic("out of vmnodes");
}

static int
vmn_doallocpg(struct vmnode *n)
{
  for(uint i = 0; i < n->npages; i++) {
    if((n->page[i] = kalloc()) == 0) {
      vmn_free(n);
      return -1;
    }
    memset((char *) n->page[i], 0, PGSIZE);
  }
  return 0;
}

struct vmnode *
vmn_allocpg(uint npg)
{
  struct vmnode *n = vmn_alloc(npg, EAGER);
  if (n == 0) return 0;
  if (vmn_doallocpg(n) < 0) return 0;
  return n;
}

void
vmn_free(struct vmnode *n)
{
  for(uint i = 0; i < n->npages; i++) {
    if (n->page[i]) {
      kfree((char *) n->page[i]);
      n->page[i] = 0;
    }
  }
  if (n->ip)
    panic("vmn_free: drop inode ref");
  n->alloc = 0;
}

void
vmn_decref(struct vmnode *n)
{
  if(__sync_sub_and_fetch(&n->ref, 1) == 0)
    vmn_free(n);
}

struct vmnode *
vmn_copy(struct vmnode *n)
{
  struct vmnode *c = vmn_allocpg(n->npages);
  if(c != 0) {
    for(uint i = 0; i < n->npages; i++) {
      memmove(c->page[i], n->page[i], PGSIZE);
    }
  }
  return c;
}

struct vmap *
vmap_alloc(void)
{
  for(uint i = 0; i < sizeof(vmaps.m) / sizeof(vmaps.m[0]); i++) {
    struct vmap *m = &vmaps.m[i];
    if(m->alloc == 0 && __sync_bool_compare_and_swap(&m->alloc, 0, 1)) {
      for(uint j = 0; j < sizeof(m->e) / sizeof(m->e[0]); j++){
	m->e[j].n = 0;
	m->e[j].va_type = PRIVATE;
	m->e[j].lock.name = "vma";
      }
      m->lock.name = "vmap";
      m->ref = 1;
      return m;
    }
  }
  panic("out of vmaps");
}

static void
vmap_free(struct vmap *m)
{
  for(uint i = 0; i < sizeof(m->e) / sizeof(m->e[0]); i++)
    if(m->e[i].n)
      vmn_decref(m->e[i].n);
  m->alloc = 0;
}

void
vmap_decref(struct vmap *m)
{
  if(__sync_sub_and_fetch(&m->ref, 1) == 0)
    vmap_free(m);
}

int
vmap_insert(struct vmap *m, struct vmnode *n, uint va_start)
{
  acquire(&m->lock);
  uint va_end = va_start + n->npages * PGSIZE;
  for(uint i = 0; i < sizeof(m->e) / sizeof(m->e[0]); i++) {
    if(m->e[i].n && (m->e[i].va_start < va_end && m->e[i].va_end > va_start)) {
      release(&m->lock);
      cprintf("vmap_insert: overlap\n");
      return -1;
    }
  }

  for(uint i = 0; i < sizeof(m->e) / sizeof(m->e[0]); i++) {
    if(m->e[i].n)
      continue;
    __sync_fetch_and_add(&n->ref, 1);
    m->e[i].va_start = va_start;
    m->e[i].va_end = va_end;
    m->e[i].n = n;
    release(&m->lock);
    return 0;
  }
  release(&m->lock);

  cprintf("vmap_insert: out of vma slots\n");
  return -1;
}

struct vma *
vmap_lookup(struct vmap *m, uint va)
{
  acquire(&m->lock);
  for(uint i = 0; i < sizeof(m->e) / sizeof(m->e[0]); i++) {
    struct vma *e = &m->e[i];
    if (va >= e->va_start && va < e->va_end) {
      acquire(&e->lock);
      release(&m->lock);
      return e;
    }
  }
  release(&m->lock);
  return 0;
}

struct vmap *
vmap_copy(struct vmap *m, pde_t* pgdir, int share)
{
  struct vmap *c = vmap_alloc();
  if(c == 0)
    return 0;

  acquire(&m->lock);
  for(uint i = 0; i < sizeof(m->e) / sizeof(m->e[0]); i++) {
    if(m->e[i].n == 0)
      continue;
    c->e[i].va_start = m->e[i].va_start;
    c->e[i].va_end = m->e[i].va_end;
    if (share) {
      c->e[i].n = m->e[i].n;
      c->e[i].va_type = COW;
      m->e[i].va_type = COW;
      updatepages(pgdir, (void *) (m->e[i].va_start), (void *) (m->e[i].va_end), PTE_COW);
    } else {
      c->e[i].n = vmn_copy(m->e[i].n);
      c->e[i].va_type = m->e[i].va_type;
    }
    if(c->e[i].n == 0) {
      release(&m->lock);
      vmap_decref(c);
      return 0;
    }
    __sync_fetch_and_add(&c->e[i].n->ref, 1);
  }
  if (share)
    lcr3(PADDR(pgdir));  // Reload hardware page table

  release(&m->lock);
  return c;
}

static int
vmn_doload(struct vmnode *vmn, struct inode *ip, uint offset, uint sz)
{
  for(uint i = 0; i < sz; i += PGSIZE){
    uint n;
    char *p = vmn->page[i / PGSIZE];
    if(sz - i < PGSIZE)
      n = sz - i;
    else
      n = PGSIZE;
    if(readi(ip, p, offset+i, n) != n)
      return -1;
  }
  return 0;
}

// Load a program segment into a vmnode.
int
vmn_load(struct vmnode *vmn, struct inode *ip, uint offset, uint sz)
{
  if (vmn->type == ONDEMAND) {
    vmn->ip = ip;
    vmn->offset = offset;
    vmn->sz = sz;
    return 0;
  } else {
    return vmn_doload(vmn, ip, offset, sz);
  }
}

// Free a page table and all the physical memory pages
// in the user part.
void
freevm(pde_t *pgdir)
{
  uint i;

  if(pgdir == 0)
    panic("freevm: no pgdir");
  for(i = 0; i < NPDENTRIES; i++){
    if(pgdir[i] & PTE_P)
      kfree((char*)PTE_ADDR(pgdir[i]));
  }
  kfree((char*)pgdir);
}

//PAGEBREAK!
// Map user virtual address to kernel physical address.
char*
uva2ka(pde_t *pgdir, char *uva)
{
  pte_t *pte;

  pte = walkpgdir(pgdir, uva, 0);
  if((*pte & PTE_P) == 0)
    return 0;
  if((*pte & PTE_U) == 0)
    return 0;
  return (char*)PTE_ADDR(*pte);
}

// Copy len bytes from p to user address va in vmap.
// Most useful when vmap is not the current page table.
int
copyout(struct vmap *vmap, uint va, void *p, uint len)
{
  char *buf = (char*)p;
  while(len > 0){
    uint va0 = (uint)PGROUNDDOWN(va);
    struct vma *vma = vmap_lookup(vmap, va);
    if(vma == 0)
      return -1;

    uint pn = (va0 - vma->va_start) / PGSIZE;
    char *p0 = vma->n->page[pn];
    if(p0 == 0)
      panic("copyout: missing page");
    uint n = PGSIZE - (va - va0);
    if(n > len)
      n = len;
    memmove(p0 + (va - va0), buf, n);
    len -= n;
    buf += n;
    va = va0 + PGSIZE;
    release(&vma->lock);
  }
  return 0;
}

int
copyin(struct vmap *vmap, uint va, void *p, uint len)
{
  char *buf = (char*)p;
  while(len > 0){
    uint va0 = (uint)PGROUNDDOWN(va);
    struct vma *vma = vmap_lookup(vmap, va);
    if(vma == 0)
      return -1;

    uint pn = (va0 - vma->va_start) / PGSIZE;
    char *p0 = vma->n->page[pn];
    if(p0 == 0)
      panic("copyout: missing page");
    uint n = PGSIZE - (va - va0);
    if(n > len)
      n = len;
    memmove(buf, p0 + (va - va0), n);
    len -= n;
    buf += n;
    va = va0 + PGSIZE;
    release(&vma->lock);
  }
  return 0;
}

int
pagefault(pde_t *pgdir, struct vmap *vmap, uint va, uint err)
{
  
  pte_t *pte = walkpgdir(pgdir, (const void *)va, 1);
  if((*pte & (PTE_P|PTE_U|PTE_W)) == (PTE_P|PTE_U|PTE_W))
    return 0;

  struct vma *m = vmap_lookup(vmap, va);
  if(m == 0)
    return -1;

  //  cprintf("%d: pf addr=0x%x err 0x%x\n", proc->pid, va, err);
  // cprintf("%d: pf vma type = %d refcnt %d  vmn type %d pte=0x%x\n", proc->pid, m->va_type, m->n->ref, m->n->type, *pte);

  uint npg = (PGROUNDDOWN(va) - m->va_start) / PGSIZE;
  if (m->n && m->n->ip && *pte == 0x0) {
    //cprintf("ODP\n");
    if (vmn_doallocpg(m->n) < 0) {
      panic("pagefault: couldn't allocate pages");
    }
    release(&m->lock);
    if (vmn_doload(m->n, m->n->ip, m->n->offset, m->n->sz) < 0) {
      panic("pagefault: couldn't load");
    }
    acquire(&m->lock);
    pte = walkpgdir(pgdir, (const void *)va, 0);
    if (pte == 0x0)
      panic("pagefault: not paged in???");
    cprintf("ODP done\n");
  }

  if (m->va_type == COW && (err & FEC_WR)) {
    // Write to a COW page
    // cprintf("write to cow\n");
    if (m->n->ref == 1) {   // if vma isn't shared any more, make it private
      m->va_type = PRIVATE;
      *pte = PADDR(m->n->page[npg]) | PTE_P | PTE_U | PTE_W;
    } else {  // vma is still shared; give process its private copy
      struct vmnode *c = vmn_copy(m->n);
      c->ref = 1;
      __sync_sub_and_fetch(&m->n->ref, 1);
      if (m->n->ref == 0) 
	panic("cow");
      m->va_type = PRIVATE;
      m->n = c;
      // Update the hardware page tables to reflect the change to the vma
      clearpages(pgdir, (void *) m->va_start, (void *) m->va_end);
      pte = walkpgdir(pgdir, (const void *)va, 0);
      *pte = PADDR(m->n->page[npg]) | PTE_P | PTE_U | PTE_W;
    }
  } else if (m->va_type == COW) {
    // cprintf("cow\n");
    *pte = PADDR(m->n->page[npg]) | PTE_P | PTE_U | PTE_COW;
  } else {
    // cprintf("fill in pte\n");
    if (m->n->ref > 1)
      panic("pagefault");
    *pte = PADDR(m->n->page[npg]) | PTE_P | PTE_U | PTE_W;
  }
  lcr3(PADDR(pgdir));  // Reload hardware page tables
  release(&m->lock);
  return 1;
}



