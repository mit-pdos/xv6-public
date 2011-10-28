// Physical memory allocator, intended to allocate
// memory for user processes, kernel stacks, page table pages,
// and pipe buffers. Allocates 4096-byte pages.

#include "types.h"
#include "param.h"
#include "mmu.h"
#include "kernel.h"
#include "spinlock.h"
#include "kalloc.h"
#include "xv6-mtrace.h"
#include "cpu.h"

struct kmem kmems[NCPU];
struct kmem kstacks[NCPU];

extern char end[]; // first address after kernel loaded from ELF file
char *newend;
enum { kalloc_memset = 0 };

static int kinited __mpalign__;

// simple page allocator to get off the ground during boot
static char *
pgalloc(void)
{
  if (newend == 0)
    newend = end;

  void *p = (void*)PGROUNDUP((uptr)newend);
  memset(p, 0, PGSIZE);
  newend = newend + PGSIZE;
  return p;
}

// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
static void
kfree_pool(struct kmem *m, char *v)
{
  struct run *r;

  if((uptr)v % PGSIZE || v < end || v2p(v) >= PHYSTOP)
    panic("kfree_pool");

  // Fill with junk to catch dangling refs.
  if (kinited && kalloc_memset)
    memset(v, 1, m->size);

  acquire(&m->lock);
  r = (struct run*)v;
  r->next = m->freelist;
  m->freelist = r;
  m->nfree++;
  if (kinited)
    mtrace_label_register(mtrace_label_block,
			  r,
			  0,
			  0,
			  0,
			  RET_EIP());
  release(&m->lock);
}

static void __attribute__((unused))
kmemprint(void)
{
  cprintf("free pages: [ ");
  for (u32 i = 0; i < NCPU; i++)
    if (i == mycpu()->id)
      cprintf("<%d> ", kmems[i].nfree);
    else
      cprintf("%d ", kmems[i].nfree);
  cprintf("]\n");
}

static char*
kmemalloc(struct kmem *km)
{
  struct run *r = 0;
  struct kmem *m;

  u32 startcpu = mycpu()->id;
  for (u32 i = 0; r == 0 && i < NCPU; i++) {
    int cn = (i + startcpu) % NCPU;
    m = &km[cn];
    acquire(&m->lock);
    r = m->freelist;
    if (r) {
      m->freelist = r->next;
      m->nfree--;
    }
    release(&m->lock);
  }

  if (r == 0) {
    cprintf("kalloc: out of memory\n");
    kmemprint();
    return 0;
  }

  mtrace_label_register(mtrace_label_block,
			r,
			m->size,
			"kalloc",
			sizeof("kalloc"),
			RET_EIP());

  if (kalloc_memset)
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
  return kmemalloc(kmems);
}

// Allocate KSTACKSIZE bytes.
char *
ksalloc(void)
{
  return kmemalloc(kstacks);
}

// Memory allocator by Kernighan and Ritchie,
// The C programming Language, 2nd ed.  Section 8.7.

typedef struct header {
  struct header *ptr;
  u64 size;   // in multiples of sizeof(Header)
} __mpalign__ Header;

static struct freelist {
  Header base;
  Header *freep;   // last allocated block
  struct spinlock lock;
  char name[MAXNAME];
} freelists[NCPU];

static void
kminit(void)
{
  for (int c = 0; c < NCPU; c++) {
    freelists[c].name[0] = (char) c + '0';
    safestrcpy(freelists[c].name+1, "freelist", MAXNAME-1);
    initlock(&freelists[c].lock, freelists[c].name);
  }
}

// Initialize free list of physical pages.
void
initkalloc(void)
{
  char *p;
  char *e;

  for (int c = 0; c < NCPU; c++) {
    kmems[c].name[0] = (char) c + '0';
    safestrcpy(kmems[c].name+1, "kmem", MAXNAME-1);
    initlock(&kmems[c].lock, kmems[c].name);
    kmems[c].size = PGSIZE;
  }

  for (int c = 0; c < NCPU; c++) {
    kstacks[c].name[0] = (char) c + '0';
    safestrcpy(kstacks[c].name+1, "kstack", MAXNAME-1);
    initlock(&kstacks[c].lock, kstacks[c].name);
    kstacks[c].size = KSTACKSIZE;
  }

  p = (char*)PGROUNDUP((uptr)newend);
  e = (char*)KBASE;
  for (int c = 0; c < NCPU; c++) {
    char *sp = p + CPUKSTACKS*KSTACKSIZE;
    char *ep = e + (PHYSTOP/NCPU);
    if (sp >= ep)
      panic("Too many stacks");
    for (; p < sp; p += KSTACKSIZE)
      kfree_pool(&kstacks[c], p);
    for (; p < ep; p += PGSIZE)
      kfree_pool(&kmems[c], p);
    e = p;
  }

  kminit();
  kinited = 1;
}

static void
domfree(void *ap)
{
  Header *bp, *p;

  bp = (Header*)ap - 1;
  if (kalloc_memset)
    memset(ap, 3, (bp->size-1) * sizeof(*bp));
  for(p = freelists[mycpu()->id].freep; !(bp > p && bp < p->ptr); p = p->ptr)
    if(p >= p->ptr && (bp > p || bp < p->ptr))
      break;
  if(bp + bp->size == p->ptr){
    bp->size += p->ptr->size;
    bp->ptr = p->ptr->ptr;
  } else
    bp->ptr = p->ptr;
  if(p + p->size == bp){
    p->size += bp->size;
    p->ptr = bp->ptr;
  } else
    p->ptr = bp;
  freelists[mycpu()->id].freep = p;
}

void
kmfree(void *ap)
{
  acquire(&freelists[mycpu()->id].lock);
  domfree(ap);
  mtrace_label_register(mtrace_label_heap,
			ap,
			0,
			0,
			0,
			RET_EIP());
  release(&freelists[mycpu()->id].lock);
}

// Caller should hold free_locky
static Header*
morecore(u64 nu)
{
  static u64 units_per_page = PGSIZE / sizeof(Header);
  char *p;
  Header *hp;

  if(nu != units_per_page) {
    if (nu > units_per_page)
      panic("morecore");
    nu = units_per_page;   // we allocate nu * sizeof(Header)
  }
  p = kalloc();
  if(p == 0)
    return 0;
  hp = (Header*)p;
  hp->size = nu;
  domfree((void*)(hp + 1));
  return freelists[mycpu()->id].freep;
}

void*
kmalloc(u64 nbytes)
{
  Header *p, *prevp;
  u64 nunits;
  void *r = 0;

  acquire(&freelists[mycpu()->id].lock);
  nunits = (nbytes + sizeof(Header) - 1)/sizeof(Header) + 1;
  if((prevp = freelists[mycpu()->id].freep) == 0){
    freelists[mycpu()->id].base.ptr =
      freelists[mycpu()->id].freep = prevp = &freelists[mycpu()->id].base;
    freelists[mycpu()->id].base.size = 0;
  }
  for(p = prevp->ptr; ; prevp = p, p = p->ptr){
    if(p->size >= nunits){
      if(p->size == nunits)
        prevp->ptr = p->ptr;
      else {
        p->size -= nunits;
        p += p->size;
        p->size = nunits;
      }
      freelists[mycpu()->id].freep = prevp;
      r = (void*)(p + 1);
      break;
    }
    if(p == freelists[mycpu()->id].freep)
      if((p = morecore(nunits)) == 0)
        break;
  }
  release(&freelists[mycpu()->id].lock);

  if (r)
    mtrace_label_register(mtrace_label_heap,
			  r,
			  nbytes,
			  "kmalloc'ed",
			  sizeof("kmalloc'ed"),
			  RET_EIP());
  return r;
}

void
kfree(void *v)
{
  kfree_pool(mykmem(), v);
}
