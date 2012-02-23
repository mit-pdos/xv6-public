//
// Physical page allocator.
// Slab allocator, for chunks larger than one page.
//

#include "types.h"
#include "mmu.h"
#include "kernel.hh"
#include "spinlock.h"
#include "kalloc.hh"
#include "mtrace.h"
#include "cpu.hh"
#include "multiboot.hh"

static struct Mbmem mem[128];
static u64 nmem;
static u64 membytes;
struct kmem kmems[NCPU];
struct kmem slabmem[slab_type_max][NCPU];

extern char end[]; // first address after kernel loaded from ELF file
char *newend;

static int kinited __mpalign__;

static struct Mbmem *
memsearch(paddr pa)
{
  struct Mbmem *e;
  struct Mbmem *q;

  q = mem+nmem;
  for (e = &mem[0]; e < q; e++)
    if ((e->base <= pa) && ((e->base+e->length) > pa))
      return e;
  return 0;
}

static u64
memsize(void *va)
{
  struct Mbmem *e;
  paddr pa = v2p(va);

  e = memsearch(pa);
  if (e == NULL)
    return -1;
  return (e->base+e->length) - pa;
}

static void *
memnext(void *va, u64 inc)
{
  struct Mbmem *e, *q;
  paddr pa = v2p(va);

  e = memsearch(pa);
  if (e == NULL)
    return (void *)-1;

  pa += inc;
  if (pa < (e->base+e->length))
    return p2v(pa);

  q = mem+nmem;
  for (e = e + 1; e < q; e++)
      return p2v(e->base);

  return (void *)-1;
}

static void
initmem(u64 mbaddr)
{
  struct Mbdata *mb;
  struct Mbmem *mbmem;
  u8 *p, *ep;

  mb = (Mbdata*) p2v(mbaddr);
  if(!(mb->flags & (1<<6)))
    panic("multiboot header has no memory map");

  p = (u8*) p2v(mb->mmap_addr);
  ep = p + mb->mmap_length;

  while (p < ep) {
    mbmem = (Mbmem *)(p+4);
    p += 4 + *(u32*)p;
    if (mbmem->type == 1) {
      membytes += mbmem->length;
      mem[nmem] = *mbmem;
      nmem++;
    }
  }
}

// simple page allocator to get off the ground during boot
static char *
pgalloc(void)
{
  if (newend == 0)
    newend = end;

  void *p = (void*)PGROUNDUP((uptr)newend);
  memset(p, 0, PGSIZE);
  newend = newend + PGSIZE;
  return (char*) p;
}

// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
static void
kfree_pool(struct kmem *m, char *v)
{
  struct run *r;

  if ((uptr)v % PGSIZE) 
    panic("kfree_pool: misaligned %p", v);
  if (v < end)
    panic("kfree_pool: less than end %p", v);
  if (memsize(v) == -1ull)
    panic("kfree_pool: unknown region %p", v);

  // Fill with junk to catch dangling refs.
  if (ALLOC_MEMSET && kinited && m->size <= 16384)
    memset(v, 1, m->size);

  r = (struct run*)v;
  r->next = m->freelist;
  while (!cmpxch_update(&m->freelist, &r->next, r))
    ; /* spin */
  m->nfree++;
  if (kinited)
    mtunlabel(mtrace_label_block, r);
}

static void __attribute__((unused))
kmemprint(void)
{
  cprintf("free pages: [ ");
  for (u32 i = 0; i < NCPU; i++)
    if (i == mycpu()->id)
      cprintf("<%lu> ", kmems[i].nfree.load());
    else
      cprintf("%lu ", kmems[i].nfree.load());
  cprintf("]\n");
}

static char*
kalloc_pool(struct kmem *km)
{
  struct run *r = 0;
  struct kmem *m;

  scoped_gc_epoch gc;

  u32 startcpu = mycpu()->id;
  for (u32 i = 0; r == 0 && i < NCPU; i++) {
    int cn = (i + startcpu) % NCPU;
    m = &km[cn];

    r = m->freelist;
    while (r && !cmpxch(&m->freelist, r, r->next))
      ; /* spin */

    if (r) {
      m->freelist = r->next;
      m->nfree--;
    }
  }

  if (r == 0) {
    cprintf("kalloc: out of memory\n");
    kmemprint();
    return 0;
  }

  mtlabel(mtrace_label_block, r, m->size, "kalloc", sizeof("kalloc"));

  if (ALLOC_MEMSET && m->size <= 16384)
    memset(r, 2, m->size);
  return (char*)r;
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
char*
kalloc(void)
{
  if (!kinited)
    return pgalloc();
  return kalloc_pool(kmems);
}

void *
ksalloc(int slab)
{
  return kalloc_pool(slabmem[slab]);
}

void
slabinit(struct kmem *k, char **p, u64 *off)
{
  for (int i = 0; i < k->ninit; i++) {
    if (*p == (void *)-1)
      panic("slabinit: memnext");
    // XXX(sbw) handle this condition
    if (memsize(p) < k->size)
      panic("slabinit: memsize");
    kfree_pool(k, *p);
    *p = (char*) memnext(*p, k->size);
    *off = *off+k->size;
  }
}  

// Initialize free list of physical pages.
void
initkalloc(u64 mbaddr)
{
  char *p;
  u64 n;
  u64 k;

  initmem(mbaddr);

  for (int c = 0; c < NCPU; c++) {
    kmems[c].name[0] = (char) c + '0';
    safestrcpy(kmems[c].name+1, "kmem", MAXNAME-1);
    kmems[c].size = PGSIZE;
  }

  for (int i = 0; i < slab_type_max; i++) {
    for (int c = 0; c < NCPU; c++) {
      slabmem[i][c].name[0] = (char) c + '0';
    }
  }

  if (VERBOSE)
    cprintf("%lu mbytes\n", membytes / (1<<20));
  n = membytes / NCPU;
  if (n & (PGSIZE-1))
    n = PGROUNDDOWN(n);

  p = (char*)PGROUNDUP((uptr)newend);
  k = (((uptr)p) - KBASE);
  for (int c = 0; c < NCPU; c++) {
    // Fill slab allocators
    strncpy(slabmem[slab_stack][c].name, "   kstack", MAXNAME);
    slabmem[slab_stack][c].size = KSTACKSIZE;
    slabmem[slab_stack][c].ninit = CPUKSTACKS;

    strncpy(slabmem[slab_perf][c].name, "   kperf", MAXNAME);
    slabmem[slab_perf][c].size = PERFSIZE;
    slabmem[slab_perf][c].ninit = 1;

    strncpy(slabmem[slab_kshared][c].name, "   kshared", MAXNAME);
    slabmem[slab_kshared][c].size = KSHAREDSIZE;
    slabmem[slab_kshared][c].ninit = CPUKSTACKS;

    for (int i = 0; i < slab_type_max; i++)
      slabinit(&slabmem[i][c], &p, &k);
   
    // The rest goes to the page allocator
    for (; k != n; k += PGSIZE, p = (char*) memnext(p, PGSIZE)) {
      if (p == (void *)-1)
        panic("initkalloc: e820next");
      kfree_pool(&kmems[c], p);
    }

    k = 0;
  }

  kminit();
  kinited = 1;
}

void
kfree(void *v)
{
  verifyfree((char*) v, mykmem()->size);
  kfree_pool(mykmem(), (char*) v);
}

void
ksfree(int slab, void *v)
{
  kfree_pool(slabmem[slab], (char*) v);
}

void
verifyfree(char *ptr, u64 nbytes)
{
#if VERIFYFREE
  char *p = ptr;
  char *e = p + nbytes;
  for (; p < e; p++) {
    // Search for pointers in the ptr region
    u64 x = *(uptr *)p;
    if (KBASE < x && x < KBASE+(128ull<<30)) {
#if 0   /* maybe once this code is C++ */
      struct klockstat *kls = (struct klockstat *) x;
      if (kls->magic == LOCKSTAT_MAGIC)
        panic("LOCKSTAT_MAGIC %p(%lu):%p->%p", 
              ptr, nbytes, p, kls);
#endif
    }
  }
#endif
}
