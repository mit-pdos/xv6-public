#include "types.h"
#include "amd64.h"
#include "mmu.h"
#include "cpu.hh"
#include "kernel.hh"
#include "bits.hh"
#include "spinlock.h"
#include "kalloc.hh"
#include "queue.h"
#include "condvar.h"
#include "proc.hh"
#include "vm.hh"
#include <stddef.h>

using namespace std;

static pgmap*
descend(pgmap *dir, u64 va, u64 flags, int create, int level)
{
  atomic<pme_t> *entryp;
  pme_t entry;
  pgmap *next;

retry:
  entryp = &dir->e[PX(level, va)];
  entry = entryp->load();
  if (entry & PTE_P) {
    next = (pgmap*) p2v(PTE_ADDR(entry));
  } else {
    if (!create)
      return NULL;
    next = (pgmap*) kalloc();
    if (!next)
      return NULL;
    memset(next, 0, PGSIZE);
    if (!cmpxch(entryp, entry, v2p(next) | PTE_P | PTE_W | flags)) {
      kfree((void*) next);
      goto retry;
    }
  }
  return next;
}

// Return the address of the PTE in page table pgdir
// that corresponds to linear address va.  If create!=0,
// create any required page table pages.
atomic<pme_t>*
walkpgdir(pgmap *pml4, u64 va, int create)
{
  auto pdp = descend(pml4, va, PTE_U, create, 3);
  if (pdp == NULL)
    return NULL;
  auto pd = descend(pdp, va, PTE_U, create, 2);
  if (pd == NULL)
    return NULL;
  auto pt = descend(pd, va, PTE_U, create, 1);
  if (pt == NULL)
    return NULL;
  return &pt->e[PX(0,va)];
}

// Map from 0 to 128Gbytes starting at KBASE.
void
initpg(void)
{
  u64 va = KBASE;
  paddr pa = 0;

  while (va < (KBASE+(128ull<<30))) {
    auto pdp = descend(&kpml4, va, 0, 1, 3);
    auto pd = descend(pdp, va, 0, 1, 2);
    atomic<pme_t> *sp = &pd->e[PX(1,va)];
    u64 flags = PTE_W | PTE_P | PTE_PS | PTE_NX;
    *sp = pa | flags;
    va += PGSIZE*512;
    pa += PGSIZE*512;
  }
}

// Set up kernel part of a page table.
pgmap*
setupkvm(void)
{
  pgmap *pml4;
  int k;

  if((pml4 = (pgmap*)kalloc()) == 0)
    return 0;
  k = PX(3, KBASE);
  memset(&pml4->e[0], 0, 8*k);
  memmove(&pml4->e[k], &kpml4.e[k], 8*(512-k));
  return pml4;
}

int
setupkshared(pgmap *pml4, char *kshared)
{
  for (u64 off = 0; off < KSHAREDSIZE; off+=4096) {
    atomic<pme_t> *pte = walkpgdir(pml4, (u64) (KSHARED+off), 1);
    if (pte == NULL)
      panic("setupkshared: oops");
    *pte = v2p(kshared+off) | PTE_P | PTE_U | PTE_W;
  }
  return 0;
}

// Switch h/w page table register to the kernel-only page table,
// for when no process is running.
void
switchkvm(void)
{
  lcr3(v2p(&kpml4));   // switch to the kernel page table
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
  if (p->vmap != 0 && p->vmap->pml4 != 0)
    lcr3(v2p(p->vmap->pml4));  // switch to new address space
  else
    switchkvm();

  popcli();
}

static void
freepm(pgmap *pm, int level)
{
  int i;

  if (level != 0) {
    for (i = 0; i < 512; i++) {
      pme_t entry = pm->e[i];
      if (entry & PTE_P)
        freepm((pgmap*) p2v(PTE_ADDR(entry)), level - 1);
    }
  }

  kfree(pm);
}

// Free a page table and all the physical memory pages
// in the user part.
void
freevm(pgmap *pml4)
{
  int k;
  int i;

  if(pml4 == 0)
    panic("freevm: no pgdir");

  // Don't free kernel portion of the pml4
  k = PX(3, KBASE);
  for (i = 0; i < k; i++) {
    pme_t entry = pml4->e[i];
    if (entry & PTE_P) {
      freepm((pgmap*) p2v(PTE_ADDR(entry)), 2);
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
  writemsr(MSR_GS_KERNBASE, (u64)&c->cpu);
  c->cpu = c;
  c->proc = NULL;
  c->kmem = &kmems[cpunum()];
}

atomic<u64> tlbflush_req;

void
tlbflush()
{
  u64 myreq = tlbflush_req++;

  // the caller may not hold any spinlock, because other CPUs might
  // be spinning waiting for that spinlock, with interrupts disabled,
  // so we will deadlock waiting for their TLB flush..
  assert(mycpu()->ncli == 0);

  for (int i = 0; i < ncpu; i++)
    lapic_tlbflush(i);

  for (int i = 0; i < ncpu; i++)
    while (cpus[i].tlbflush_done < myreq)
      /* spin */ ;
}
