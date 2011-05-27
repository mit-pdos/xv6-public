// Physical memory allocator, intended to allocate
// memory for user processes, kernel stacks, page table pages,
// and pipe buffers. Allocates 4096-byte pages.

#include "types.h"
#include "defs.h"
#include "param.h"
#include "mmu.h"
#include "spinlock.h"
#include "condvar.h"
#include "queue.h"
#include "proc.h"
#include "kalloc.h"
#include "xv6-mtrace.h"

void kminit(void);

struct kmem kmems[NCPU];
extern char end[]; // first address after kernel loaded from ELF file

static void __attribute__((unused))
kmemprint(void)
{
  cprintf("free pages: [ ");
  for (uint i = 0; i < NCPU; i++)
    if (i == cpunum())
      cprintf("<%d> ", kmems[i].nfree);
    else
      cprintf("%d ", kmems[i].nfree);
  cprintf("]\n");
}

// Initialize free list of physical pages.
void
kinit(void)
{
  char *p;

  for (int c = 0; c < NCPU; c++) {
    kmems[c].name[0] = (char) c + '0';
    safestrcpy(kmems[c].name+1, "kmem", MAXNAME-1);
    initlock(&kmems[c].lock, kmems[c].name);
  }

  p = (char*)PGROUNDUP((uint)end);
  for(; p + PGSIZE <= (char*)PHYSTOP; p += PGSIZE)
    kfree(p);
  kminit();
}

//PAGEBREAK: 21
// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(char *v)
{
  struct run *r;

  if((uint)v % PGSIZE || v < end || (uint)v >= PHYSTOP)
    panic("kfree");

  // Fill with junk to catch dangling refs.
  memset(v, 1, PGSIZE);

  struct kmem *m = kmem;
  acquire(&m->lock);
  r = (struct run*)v;
  r->next = m->freelist;
  m->freelist = r;
  m->nfree++;
  mtrace_label_register(mtrace_label_block,
			r,
			0,
			0,
			0,
			RET_EIP());

  release(&m->lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
char*
kalloc(void)
{
  struct run *r = 0;

  //  cprintf("%d: kalloc 0x%x 0x%x 0x%x 0x%x 0%x\n", cpu->id, kmem, &kmems[cpu->id], kmem->freelist, PHYSTOP, kmems[1].freelist);

  uint startcpu = cpunum();
  for (uint i = 0; r == 0 && i < NCPU; i++) {
    int cn = (i + startcpu) % NCPU;
    struct kmem *m = &kmems[cn];
    acquire(&m->lock);
    r = m->freelist;
    if (r) {
      m->freelist = r->next;
      m->nfree--;
    }
    release(&m->lock);
  }

  if (r == 0) {
    cprintf("kalloc: out of memory");
    kmemprint();
    return 0;
  }

  mtrace_label_register(mtrace_label_block,
			r,
			4096,
			"kalloc",
			sizeof("kalloc"),
			RET_EIP());

  memset(r, 2, PGSIZE);
  return (char*)r;
}


// Memory allocator by Kernighan and Ritchie,
// The C programming Language, 2nd ed.  Section 8.7.

typedef long Align;

union header {
  struct {
    union header *ptr;
    uint size;   // in multiples of sizeof(Header)
  } s;
  Align x;
};

typedef union header Header;

static struct freelist {
  Header base;
  Header *freep;   // last allocated block
  struct spinlock lock;
  char name[MAXNAME];
} freelists[NCPU];

void
kminit(void)
{
  for (int c = 0; c < NCPU; c++) {
    freelists[c].name[0] = (char) c + '0';
    safestrcpy(freelists[c].name+1, "freelist", MAXNAME-1);
    initlock(&freelists[c].lock, freelists[c].name);
  }
}

static void
domfree(void *ap)
{
  Header *bp, *p;

  bp = (Header*)ap - 1;
  for(p = freelists[cpu->id].freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
    if(p >= p->s.ptr && (bp > p || bp < p->s.ptr))
      break;
  if(bp + bp->s.size == p->s.ptr){
    bp->s.size += p->s.ptr->s.size;
    bp->s.ptr = p->s.ptr->s.ptr;
  } else
    bp->s.ptr = p->s.ptr;
  if(p + p->s.size == bp){
    p->s.size += bp->s.size;
    p->s.ptr = bp->s.ptr;
  } else
    p->s.ptr = bp;
  freelists[cpu->id].freep = p;
}

void
kmfree(void *ap)
{
  acquire(&freelists[cpu->id].lock);
  domfree(ap);
  release(&freelists[cpu->id].lock);
}

// Caller should hold free_locky
static Header*
morecore(uint nu)
{
  char *p;
  Header *hp;

  if(nu != 512) {
    if (nu > 512)
      panic("morecore");
    nu = 512;   // we allocate nu * sizeof(Header)
  }
  p = kalloc();
  if(p == 0)
    return 0;
  hp = (Header*)p;
  hp->s.size = nu;
  domfree((void*)(hp + 1));
  return freelists[cpu->id].freep;
}

void*
kmalloc(uint nbytes)
{
  Header *p, *prevp;
  uint nunits;
  void *r = 0;

  acquire(&freelists[cpu->id].lock);
  nunits = (nbytes + sizeof(Header) - 1)/sizeof(Header) + 1;
  if((prevp = freelists[cpu->id].freep) == 0){
    freelists[cpu->id].base.s.ptr = freelists[cpu->id].freep = prevp = &freelists[cpu->id].base;
    freelists[cpu->id].base.s.size = 0;
  }
  for(p = prevp->s.ptr; ; prevp = p, p = p->s.ptr){
    if(p->s.size >= nunits){
      if(p->s.size == nunits)
        prevp->s.ptr = p->s.ptr;
      else {
        p->s.size -= nunits;
        p += p->s.size;
        p->s.size = nunits;
      }
      freelists[cpu->id].freep = prevp;
      r = (void*)(p + 1);
      break;
    }
    if(p == freelists[cpu->id].freep)
      if((p = morecore(nunits)) == 0)
        break;
  }
  release(&freelists[cpu->id].lock);
  return r;
}
