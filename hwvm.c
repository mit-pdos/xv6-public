#include "types.h"
#include "amd64.h"
#include "mmu.h"
#include "cpu.h"
#include "kernel.h"
#include "bits.h"
#include "spinlock.h"
#include "kalloc.h"
#include "queue.h"
#include "condvar.h"
#include "proc.h"
#include "vm.h"
#include <stddef.h>

extern pml4e_t kpml4[];

static pme_t*
descend(pme_t *dir, const void *va, u64 flags, int create, int level)
{
  pme_t entry;
  pme_t *next;

retry:
  dir = &dir[PX(level, va)];
  entry = *dir;
  if (entry & PTE_P) {
    next = p2v(PTE_ADDR(entry));
  } else {
    if (!create)
      return NULL;
    next = (pme_t*) kalloc();
    if (!next)
      return NULL;
    memset(next, 0, PGSIZE);
    if (!cmpswap(dir, entry, v2p(next) | PTE_P | PTE_W | flags)) {
      kfree((void*) next);
      goto retry;
    }
  }
  
  return next;
}

// Return the address of the PTE in page table pgdir
// that corresponds to linear address va.  If create!=0,
// create any required page table pages.
pme_t *
walkpgdir(pml4e_t *pml4, const void *va, int create)
{
  pme_t *pdp;
  pme_t *pd;
  pme_t *pt;

  pdp = descend(pml4, va, PTE_U, create, 3);
  if (pdp == NULL)
    return NULL;
  pd = descend(pdp, va, PTE_U, create, 2);
  if (pd == NULL)
    return NULL;
  pt = descend(pd, va, PTE_U, create, 1);
  if (pt == NULL)
    return NULL;
  return &pt[PX(0,va)];
}

void
updatepages(pme_t *pml4, void *begin, void *end, int perm)
{
  char *a, *last;
  pme_t *pte;

  a = PGROUNDDOWN(begin);
  last = PGROUNDDOWN(end);
  for (;;) {
    pte = walkpgdir(pml4, a, 1);
    if(pte != 0) {
      if (perm == 0) *pte = 0;
      else *pte = PTE_ADDR(*pte) | perm | PTE_P;
    }
    if (a == last)
      break;
    a += PGSIZE;
  }
}

static void
pgmap(void *va, void *last, paddr pa)
{
  pme_t *pdp;
  pme_t *pd;
  pme_t *pt;

  for (;;) {
    pdp = descend(kpml4, va, 0, 1, 3);
    pd = descend(pdp, va, 0, 1, 2);
    pt = descend(pd, va, 0, 1, 1);
    pt = &pt[PX(0,va)];
    *pt = pa | PTE_W | PTE_P;
    if(va == last)
      break;
    va += PGSIZE;
    pa += PGSIZE;
  }
}

// set up a page table to get off the ground
void
initpg(char* (*alloc)(void))
{
  // Map first 4GB to KBASE
  pgmap((void *) (KBASE+(1UL<<30)), (void *) (KBASE+(1UL<<32)), (1UL<<30));
  // boot.S maps first 1GB to KBASE and gets us running with kpml4
}

// Set up kernel part of a page table.
pml4e_t*
setupkvm(void)
{
  pml4e_t *pml4;
  int k;

  if((pml4 = (pml4e_t*)kalloc()) == 0)
    return 0;
  k = PX(3, KBASE);
  memset(&pml4[0], 0, 8*k);
  memmove(&pml4[k], &kpml4[k], 8*(512-k));
  return pml4;
}

// Switch h/w page table register to the kernel-only page table,
// for when no process is running.
void
switchkvm(void)
{
  lcr3(v2p(kpml4));   // switch to the kernel page table
}

// Switch TSS and h/w page table to correspond to process p.
void
switchuvm(struct proc *p)
{
  u64 base = (u64) &mycpu()->ts;
  pushcli();
  mycpu()->gdt[TSSSEG>>3] = (struct segdesc)
    SEGDESC(base, (sizeof(mycpu()->ts)-1), SEG_P|SEG_TSS64A);
  mycpu()->gdt[(TSSSEG>>3)+1] = (struct segdesc) SEGDESCHI(base);
  mycpu()->ts.rsp[0] = (u64) myproc()->kstack + KSTACKSIZE;
  mycpu()->ts.iomba = (u16)offsetof(struct taskstate, iopb);
  ltr(TSSSEG);
  if(p->vmap == 0 || p->vmap->pml4 == 0)
    panic("switchuvm: no vmap/pml4");
  lcr3(v2p(p->vmap->pml4));  // switch to new address space
  popcli();
}

static void
freepm(pme_t *pm, int level)
{
  int i;

  if (level != 0) {
    for (i = 0; i < 512; i++) {
      if (pm[i] & PTE_P)
        freepm(p2v(PTE_ADDR(pm[i])), level - 1);
    }
  }

  kfree(pm);
}

// Free a page table and all the physical memory pages
// in the user part.
void
freevm(pml4e_t *pml4)
{
  int k;
  int i;

  if(pml4 == 0)
    panic("freevm: no pgdir");

  // Don't free kernel portion of the pml4
  k = PX(3, KBASE);
  for (i = 0; i < k; i++) {
    if (pml4[i] & PTE_P) {
      freepm(p2v(PTE_ADDR(pml4[i])), 2);
    }
  }
  
  kfree(pml4);
}

// Set up CPU's kernel segment descriptors.
// Run once at boot time on each CPU.
void
inittls(void)
{
  struct cpu *c;

  // Initialize cpu-local storage.
  c = &cpus[cpunum()];
  writegs(KDSEG);
  writemsr(MSR_GS_BASE, (u64)&c->cpu);
  c->cpu = c;
  c->proc = NULL;
  c->kmem = &kmems[cpunum()];
}
