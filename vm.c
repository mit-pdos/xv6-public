#include "param.h"
#include "types.h"
#include "defs.h"
#include "x86.h"
#include "memlayout.h"
#include "mmu.h"
#include "spinlock.h"
#include "condvar.h"
#include "queue.h"
#include "proc.h"
#include "elf.h"
#include "kalloc.h"

extern char data[];  // defined in data.S

static pde_t *kpgdir __attribute__ ((aligned (CACHELINE)));  // for use in scheduler()

struct segdesc gdt[NSEGS];

// page map for during boot
static void
pgmap(void *va, void *last, uint pa)
{
  pde_t *pde;
  pte_t *pgtab;
  pte_t *pte;

  for(;;){
    pde = &kpgdir[PDX(va)];
    pde_t pdev = *pde;
    if (pdev == 0) {
      pgtab = (pte_t *) pgalloc();
      *pde = v2p(pgtab) | PTE_P | PTE_W;
    } else {
      pgtab = (pte_t*)p2v(PTE_ADDR(pdev));
    }
    pte = &pgtab[PTX(va)];
    *pte = pa | PTE_W | PTE_P;
    if(va == last)
      break;
    va += PGSIZE;
    pa += PGSIZE;
  }
}

// set up a page table to get off the ground
void
pginit(char* (*alloc)(void))
{
  uint cr0;

  kpgdir = (pde_t *) alloc();
  pgmap((void *) 0, (void *) PHYSTOP, 0);
  pgmap((void *) KERNBASE, (void *) (KERNBASE+PHYSTOP), 0);
  pgmap((void*)0xFE000000, 0, 0xFE000000);

  switchkvm(); // load kpgdir into cr3

  cr0 = rcr0();
  cr0 |= CR0_PG;
  lcr0(cr0);   // paging on

  // new gdt
  gdt[SEG_KCODE] = SEG(STA_X|STA_R, 0, 0xffffffff, 0);
  gdt[SEG_KDATA] = SEG(STA_W, 0, 0xffffffff, 0);
  lgdt((void *)v2p(gdt), sizeof(gdt));
  loadgs(SEG_KDATA << 3);
  loadfs(SEG_KDATA << 3);
  loades(SEG_KDATA << 3);
  loadds(SEG_KDATA << 3);
  loadss(SEG_KDATA << 3);

  __asm volatile("ljmp %0,$1f\n 1:\n" :: "i" (SEG_KCODE << 3));  // reload cs
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

  // Map cpu, curproc, kmem
  c->gdt[SEG_KCPU] = SEG(STA_W, &c->cpu, 12, 0);

  // lgt((void *) v2p((void*)(c->gdt)), sizeof(c->gdt));
  lgdt((void *)(c->gdt), sizeof(c->gdt));
  loadgs(SEG_KCPU << 3);
  
  // Initialize cpu-local storage.
  cpu = c;
  proc = 0;
  kmem = &kmems[cpunum()];
}

void
printpgdir(pde_t *pgdir)
{
  pde_t *pde;
  pte_t *pgtab;
  pte_t pte;

  for (uint i = 0; i < NPDENTRIES; i++) {
    pde = &pgdir[i];
    pde_t pdev = *pde;
    if (pdev & PTE_P) {
      pgtab = (pte_t*)p2v(PTE_ADDR(pdev));
      cprintf("%d: p 0x%x v 0x%x\n", i, PTE_ADDR(pdev), pgtab);
      for (uint j = 0; j < NPTENTRIES; j++) {
	pte = pgtab[j];
	if (pte & PTE_P) {
	  void *pg = p2v(PTE_ADDR(pte));
	  if (pg != 0)
	    ; // cprintf("  %d: 0x%x v 0x%x\n", j, pte, pg);
	}
      }
    }
  }
}

// Return the address of the PTE in page table pgdir
// that corresponds to linear address va.  If create!=0,
// create any required page table pages.
static pte_t *
walkpgdir(pde_t *pgdir, const void *va, int create)
{
  pde_t *pde;
  pte_t *pgtab;

 retry:
  pde = &pgdir[PDX(va)];
  pde_t pdev = *pde;
  if(pdev & PTE_P){
    pgtab = (pte_t*)p2v(PTE_ADDR(pdev));
  } else {
    if(!create || (pgtab = (pte_t*)kalloc()) == 0)
      return 0;
    // Make sure all those PTE_P bits are zero.
    memset(pgtab, 0, PGSIZE);
    // The permissions here are overly generous, but they can
    // be further restricted by the permissions in the page table 
    // entries, if necessary.
    if (!__sync_bool_compare_and_swap(pde, pdev, v2p(pgtab) | PTE_P | PTE_W | PTE_U)) {
      kfree((void*) pgtab);
      goto retry;
    }
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

static void
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
}

void
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
}

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
  void *l;
  uint p;
  uint e;
  int perm;
} kmap[] = {
  { (void *)IOSPACEB, IOSPACEB, IOSPACEE, PTE_W},  // I/O space
  { P2V(IOSPACEB), IOSPACEB, IOSPACEE, PTE_W},  // I/O space
  { (void *)KERNLINK, V2P(KERNLINK), V2P(data),  0},  // kernel text, rodata
  { data, V2P(data), PHYSTOP,  PTE_W},  // kernel data, memory
  { (void*)0xFE000000, 0xFE000000, 0, PTE_W},  // device mappings
};

// Set up kernel part of a page table.
static pde_t*
setupkvm(void)
{
  pde_t *pgdir;
  struct kmap *k;

  if((pgdir = (pde_t*)kalloc()) == 0)
    return 0;
  memset(pgdir, 0, PGSIZE);
  k = kmap;
  for(k = kmap; k < &kmap[NELEM(kmap)]; k++)
    if(mappages(pgdir, k->l, k->e - k->p, (uint)k->p, k->perm) < 0)
      return 0;

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

// Turn on paging.
void
vmenable(void)
{
  uint cr0;

  switchkvm(); // load kpgdir into cr3

  cr0 = rcr0();
  cr0 |= CR0_PG;
  lcr0(cr0);

  struct cpu *c = &cpus[0];
  lgdt((void *)v2p((void *)(c->gdt)), sizeof(c->gdt));
  loadgs(SEG_KCPU << 3);
  loadfs(SEG_KDATA << 3);
  loades(SEG_KDATA << 3);
  loadds(SEG_KDATA << 3);
  loadss(SEG_KDATA << 3);

  __asm volatile("ljmp %0,$1f\n 1:\n" :: "i" (SEG_KCODE << 3));  // reload cs
}

// Switch h/w page table register to the kernel-only page table,
// for when no process is running.
void
switchkvm(void)
{
  lcr3(v2p(kpgdir));   // switch to the kernel page table
}

// Switch TSS and h/w page table to correspond to process p.
void
switchuvm(struct proc *p)
{
  pushcli();
  cpu->gdt[SEG_TSS] = SEG16(STS_T32A, &cpu->ts, sizeof(cpu->ts)-1, 0);
  cpu->gdt[SEG_TSS].s = 0;
  cpu->ts.ss0 = SEG_KDATA << 3;
  cpu->ts.esp0 = (uint) proc->kstack + KSTACKSIZE;
  ltr(SEG_TSS << 3);
  if(p->vmap == 0 || p->vmap->pgdir == 0)
    panic("switchuvm: no vmap/pgdir");
  lcr3(v2p(p->vmap->pgdir));  // switch to new address space
  popcli();
}

// Free a page table and all the physical memory pages
// in the user part.
static void
freevm(pde_t *pgdir)
{
  uint i;

  if(pgdir == 0)
    panic("freevm: no pgdir");
  for(i = 0; i < NPDENTRIES; i++){
    if(pgdir[i] & PTE_P)
      kfree(p2v(PTE_ADDR(pgdir[i])));
  }
  kfree(pgdir);
}

struct vmnode *
vmn_alloc(uint npg, uint type)
{
  struct vmnode *n = kmalloc(sizeof(struct vmnode));
  if (n == 0) {
    cprintf("out of vmnodes");
    return 0;
  }
  if(npg > NELEM(n->page)) {
    panic("vmnode too big\n");
  }
  memset(n, 0, sizeof(struct vmnode));
  n->npages = npg;
  n->type = type;
  return n;
}

static int
vmn_doallocpg(struct vmnode *n)
{
  for(uint i = 0; i < n->npages; i++) {
    if((n->page[i] = kalloc()) == 0)
      return -1;
    memset((char *) n->page[i], 0, PGSIZE);
  }
  return 0;
}

struct vmnode *
vmn_allocpg(uint npg)
{
  struct vmnode *n = vmn_alloc(npg, EAGER);
  if (n == 0) return 0;
  if (vmn_doallocpg(n) < 0) {
    vmn_free(n);
    return 0;
  }
  return n;
}

void
vmn_free(struct vmnode *n)
{
  for(uint i = 0; i < n->npages; i++) {
    if (n->page[i]) {
      kfree(n->page[i]);   // vmn_free is rcu delayed, when kfree must be delayed, don't delay
      n->page[i] = 0;
    }
  }
  if (n->ip)
    iput(n->ip);
  n->ip = 0;
  kmfree(n);   // vmn_free is rcu delayed, when kmfree must be delayed, don't delay again
}

static void
vmn_decref(struct vmnode *n)
{
  if(__sync_sub_and_fetch(&n->ref, 1) == 0)
    vmn_free(n);   // XXX delay?
}

struct vmnode *
vmn_copy(struct vmnode *n)
{
  struct vmnode *c = vmn_alloc(n->npages, n->type);
  if(c != 0) {
    c->type = n->type;
    if (n->type == ONDEMAND) {
      c->ip = idup(n->ip);
      c->offset = n->offset;
      c->sz = c->sz;
    } 
    if (n->page[0]) {   // If the first page is present, all of them are present
      if (vmn_doallocpg(c) < 0) {
	cprintf("vmn_copy: out of memory\n");
	vmn_free(c);
	cprintf("return\n");
	return 0;
      }
      for(uint i = 0; i < n->npages; i++) {
	memmove(c->page[i], n->page[i], PGSIZE);
      }
    }
  }
  return c;
}

static struct vma *
vma_alloc(void)
{
  struct vma *e = kmalloc(sizeof(struct vma));
  if (e == 0)
    return 0;
  memset(e, 0, sizeof(struct vma));
  e->va_type = PRIVATE;
  snprintf(e->lockname, sizeof(e->lockname), "vma:%p", e);
  initlock(&e->lock, e->lockname);
  return e;
}

struct vmap *
vmap_alloc(void)
{
  struct vmap *m = kmalloc(sizeof(struct vmap));
  if (m == 0)
    return 0;

  memset(m, 0, sizeof(struct vmap));
  snprintf(m->lockname, sizeof(m->lockname), "vmap:%p", m);
  initlock(&m->lock, m->lockname);
  m->ref = 1;
  m->pgdir = setupkvm();
  if (m->pgdir == 0) {
    cprintf("vmap_alloc: setupkvm out of memory\n");
    kmfree(m);
    return 0;
  }
  return m;
}

static void
vma_free(void *p)
{
  struct vma *e = (struct vma *) p;
  if(e->n)
    vmn_decref(e->n);
  kmfree(e);  // already rcu delayed
}

static void
vmap_free(void *p)
{
  struct vmap *m = (struct vmap *) p;
  for(uint i = 0; i < NELEM(m->e); i++) {
    if (m->e[i])
      vma_free(m->e[i]);  // already rcu delayed
  }
  freevm(m->pgdir);  // already rcu delayed
  m->pgdir = 0;
  m->alloc = 0;
}

void
vmap_decref(struct vmap *m)
{
  if(__sync_sub_and_fetch(&m->ref, 1) == 0)
    rcu_delayed(m, vmap_free);
}

// Does any vma overlap start..start+len?
// If yes, return the m->e[] index.
// If no, return -1.
// This code can't handle regions at the very end
// of the address space, e.g. 0xffffffff..0x0
int
vmap_overlap(struct vmap *m, uint start, uint len)
{
#if SPINLOCK_DEBUG
  if(holding(&m->lock) == 0)
    panic("vmap_overlap no lock");
#endif
  if(start + len < start)
    panic("vmap_overlap bad len");

  for(uint i = 0; i < NELEM(m->e); i++){
    if(m->e[i]){
      if(m->e[i]->va_end <= m->e[i]->va_start)
        panic("vmap_overlap bad vma");
      if(m->e[i]->va_start < start+len && m->e[i]->va_end > start)
        return i;
    }
  }
  return -1;
}

int
vmap_insert(struct vmap *m, struct vmnode *n, uint va_start)
{
  acquire(&m->lock);
  uint len = n->npages * PGSIZE;

  if(vmap_overlap(m, va_start, len) != -1){
    release(&m->lock);
    cprintf("vmap_insert: overlap\n");
    return -1;
  }

  for(uint i = 0; i < NELEM(m->e); i++) {
    if(m->e[i])
      continue;
    m->e[i] = vma_alloc();
    if (m->e[i] == 0)
      return -1;
    m->e[i]->va_start = va_start;
    m->e[i]->va_end = va_start + len;
    m->e[i]->n = n;
    __sync_fetch_and_add(&n->ref, 1);
    release(&m->lock);
    return 0;
  }
  release(&m->lock);

  cprintf("vmap_insert: out of vma slots\n");
  return -1;
}

int
vmap_remove(struct vmap *m, uint va_start, uint len)
{
  acquire(&m->lock);
  uint va_end = va_start + len;
  for(uint i = 0; i < NELEM(m->e); i++) {
    if(m->e[i] && (m->e[i]->va_start < va_end && m->e[i]->va_end > va_start)) {
      if(m->e[i]->va_start != va_start || m->e[i]->va_end != va_end) {
	release(&m->lock);
	cprintf("vmap_remove: partial unmap unsupported\n");
	return -1;
      }
      rcu_delayed(m->e[i], vma_free);
      m->e[i] = 0;
    }
  }
  release(&m->lock);
  return 0;
}

struct vma *
vmap_lookup(struct vmap *m, uint va)
{
  acquire(&m->lock); // rcu read
  int ind = vmap_overlap(m, va, 1);
  if(ind >= 0){
    struct vma *e = m->e[ind];
    acquire(&e->lock);
    release(&m->lock);
    return e;
  }
  release(&m->lock);
  return 0;
}

struct vma *
vmap_lookup_rcu(struct vmap *m, uint va)
{
  rcu_begin_read();
  int ind = vmap_overlap(m, va, 1);
  if(ind >= 0){
    struct vma *e = m->e[ind];
    return e;
  }
  return 0;
}

struct vmap *
vmap_copy(struct vmap *m, int share)
{
  struct vmap *c = vmap_alloc();
  if(c == 0)
    return 0;

  acquire(&m->lock);
  for(uint i = 0; i < NELEM(m->e); i++) {
    if(m->e[i] == 0)
      continue;
    c->e[i] = vma_alloc();
    if (c->e[i] == 0) {
      release(&m->lock);
      vmap_free(c);
      return 0;
    }
    c->e[i]->va_start = m->e[i]->va_start;
    c->e[i]->va_end = m->e[i]->va_end;
    if (share) {
      c->e[i]->n = m->e[i]->n;
      c->e[i]->va_type = COW;
      m->e[i]->va_type = COW;
      updatepages(m->pgdir, (void *) (m->e[i]->va_start), (void *) (m->e[i]->va_end), PTE_COW);
    } else {
      c->e[i]->n = vmn_copy(m->e[i]->n);
      c->e[i]->va_type = m->e[i]->va_type;
    }
    if(c->e[i]->n == 0) {
      release(&m->lock);
      vmap_free(c);
      return 0;
    }
    __sync_fetch_and_add(&c->e[i]->n->ref, 1);
  }
  if (share)
    lcr3(v2p(m->pgdir));  // Reload hardware page table

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

static struct vma *
pagefault_ondemand(struct vmap *vmap, uint va, uint err, struct vma *m)
{
  if (vmn_doallocpg(m->n) < 0) {
    panic("pagefault: couldn't allocate pages");
  }
  release(&m->lock);
  if (vmn_doload(m->n, m->n->ip, m->n->offset, m->n->sz) < 0) {
    panic("pagefault: couldn't load");
  }
  m = vmap_lookup(vmap, va);   // re-acquire lock on m
  return m;
}

static int
pagefault_wcow(struct vmap *vmap, uint va, pte_t *pte, struct vma *m, uint npg)
{
  // Always make a copy of n, even if this process has the only ref, because other processes
  // may change ref count while this process is handling wcow
  struct vmnode *n = m->n;
  struct vmnode *c = vmn_copy(m->n);
  if (c == 0) {
    cprintf("pagefault_wcow: out of mem\n");
    return -1;
  }
  c->ref = 1;
  m->va_type = PRIVATE;
  m->n = c;
  // Update the hardware page tables to reflect the change to the vma
  clearpages(vmap->pgdir, (void *) m->va_start, (void *) m->va_end);
  pte = walkpgdir(vmap->pgdir, (const void *)va, 0);
  *pte = v2p(m->n->page[npg]) | PTE_P | PTE_U | PTE_W;
  // drop my ref to vmnode
  vmn_decref(n);
  return 0;
}

int
pagefault(struct vmap *vmap, uint va, uint err)
{
  pte_t *pte = walkpgdir(vmap->pgdir, (const void *)va, 1);
  if((*pte & (PTE_P|PTE_U|PTE_W)) == (PTE_P|PTE_U|PTE_W))
    return 0;
  struct vma *m = vmap_lookup(vmap, va);
  if(m == 0)
    return -1;

  uint npg = (PGROUNDDOWN(va) - m->va_start) / PGSIZE;
  if (m->n && m->n->type == ONDEMAND && m->n->page[npg] == 0) {
    m = pagefault_ondemand(vmap, va, err, m);
  }
  if (m->va_type == COW && (err & FEC_WR)) {
    if (pagefault_wcow(vmap, va, pte, m, npg) < 0) {
      release(&m->lock);
      return -1;
    }
  } else if (m->va_type == COW) {
    *pte = v2p(m->n->page[npg]) | PTE_P | PTE_U | PTE_COW;
  } else {
    if (m->n->ref != 1) {
      panic("pagefault");
    }
    *pte = v2p(m->n->page[npg]) | PTE_P | PTE_U | PTE_W;
  }
  lcr3(v2p(vmap->pgdir));  // Reload hardware page tables
  release(&m->lock);
  return 1;
}
