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
#include "wq.hh"
#include "apic.hh"

using namespace std;

static const char *levelnames[] = {
  "PT", "PD", "PDP", "PML4"
};

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
      return nullptr;
    next = (pgmap*) kalloc(levelnames[level-1]);
    if (!next)
      return nullptr;
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
  if (pdp == nullptr)
    return nullptr;
  auto pd = descend(pdp, va, PTE_U, create, 2);
  if (pd == nullptr)
    return nullptr;
  auto pt = descend(pd, va, PTE_U, create, 1);
  if (pt == nullptr)
    return nullptr;
  return &pt->e[PX(0,va)];
}

// Map from 0 to 128Gbytes starting at KBASE.
void
initpg(void)
{
  u64 va = KBASE;
  paddr pa = 0;

  while (va < KBASEEND) {
    auto pdp = descend(&kpml4, va, 0, 1, 3);
    assert(pdp);

    auto pd = descend(pdp, va, 0, 1, 2);
    assert(pd);

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

  if((pml4 = (pgmap*)kalloc("PML4")) == 0)
    return 0;
  k = PX(3, KBASE);
  memset(&pml4->e[0], 0, 8*k);
  memmove(&pml4->e[k], &kpml4.e[k], 8*(512-k));
  return pml4;
}

int
mapkva(pgmap *pml4, char* kva, uptr uva, size_t size)
{
  for (u64 off = 0; off < size; off+=4096) {
    atomic<pme_t> *pte = walkpgdir(pml4, (u64) (uva+off), 1);
    if (pte == nullptr)
      return -1;
    *pte = v2p(kva+off) | PTE_P | PTE_U | PTE_W;
  }
  return 0;
}

int
setupuvm(pgmap *pml4, char *kshared, char *uwq)
{
  struct todo {
    char *kvm;
    char *uvm;
    size_t size;
  } todo[] = {
    { kshared, (char*)KSHARED, KSHAREDSIZE },
    { uwq,     (char*)USERWQ,  USERWQSIZE }
  };

  for (int i = 0; i < NELEM(todo); i++) {
    for (u64 off = 0; off < todo[i].size; off+=4096) {
      atomic<pme_t> *pte = walkpgdir(pml4, (u64) (todo[i].uvm+off), 1);
      if (pte == nullptr)
        return -1;
      *pte = v2p(todo[i].kvm+off) | PTE_P | PTE_U | PTE_W;
    }
  }
  return 0;
}

// Switch h/w page table register to the kernel-only page table,
// for when no process is running.
static void
switchkvm(void)
{
  lcr3(v2p(&kpml4));   // switch to the kernel page table
}

// Switch TSS and h/w page table to correspond to process p.
void
switchvm(struct proc *p)
{
  u64 base = (u64) &mycpu()->ts;
  pushcli();
  mycpu()->gdt[TSSSEG>>3] = (struct segdesc)
    SEGDESC(base, (sizeof(mycpu()->ts)-1), SEG_P|SEG_TSS64A);
  mycpu()->gdt[(TSSSEG>>3)+1] = (struct segdesc) SEGDESCHI(base);
  mycpu()->ts.rsp[0] = (u64) myproc()->kstack + KSTACKSIZE;
  mycpu()->ts.iomba = (u16)__offsetof(struct taskstate, iopb);
  ltr(TSSSEG);

  u64 nreq = tlbflush_req.load();
  if (p->vmap != 0 && p->vmap->pml4 != 0)
    lcr3(v2p(p->vmap->pml4));  // switch to new address space
  else
    switchkvm();
  mycpu()->tlbflush_done = nreq;

  writefs(UDSEG);
  writemsr(MSR_FS_BASE, p->user_fs_);

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
  cpuid_t id = -1;

  for (id = 0; id < NCPU; id++)
    if (cpus[id].hwid.num == lapicid().num)
      break;
  assert(id != -1);

  // Initialize cpu-local storage.
  c = &cpus[id];
  writegs(KDSEG);
  writemsr(MSR_GS_BASE, (u64)&c->cpu);
  writemsr(MSR_GS_KERNBASE, (u64)&c->cpu);
  c->cpu = c;
  c->proc = nullptr;
  c->kmem = &kmems[id];
}

atomic<u64> tlbflush_req;

void
tlbflush()
{
  u64 myreq = ++tlbflush_req;
  tlbflush(myreq);
}

void
tlbflush(u64 myreq)
{
  // the caller may not hold any spinlock, because other CPUs might
  // be spinning waiting for that spinlock, with interrupts disabled,
  // so we will deadlock waiting for their TLB flush..
  assert(mycpu()->ncli == 0);

  for (int i = 0; i < ncpu; i++)
    if (cpus[i].tlbflush_done < myreq)
      lapic_tlbflush(cpus[i].hwid);

  for (int i = 0; i < ncpu; i++)
    while (cpus[i].tlbflush_done < myreq)
      /* spin */ ;
}
