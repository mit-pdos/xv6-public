// Physical memory allocator, intended to allocate
// memory for user processes, kernel stacks, page table pages,
// and pipe buffers. Allocates 4096-byte pages.

#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "spinlock.h"

void freerange(void *vstart, void *vend);
extern char end[]; // first address after kernel loaded from ELF file

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  int use_lock;
  struct run *freelist;
  uint free_pages; // A int variable to store the number of free pages
  uint pg_refcount[PHYSTOP >> PGSHIFT];
} kmem;

// Initialization happens in two phases.
// 1. main() calls kinit1() while still using entrypgdir to place just
// the pages mapped by entrypgdir on free list.
// 2. main() calls kinit2() with the rest of the physical pages
// after installing a full page table that maps them on all cores.
void
kinit1(void *vstart, void *vend)
{
  initlock(&kmem.lock, "kmem");
  kmem.use_lock = 0;
  kmem.free_pages = 0;
  freerange(vstart, vend);
}

void
kinit2(void *vstart, void *vend)
{
  freerange(vstart, vend);
  kmem.use_lock = 1;
}

void
freerange(void *vstart, void *vend)
{
  char *p;
  p = (char*)PGROUNDUP((uint)vstart);
  for(; p + PGSIZE <= (char*)vend; p += PGSIZE){
    kmem.pg_refcount[V2P(p) >> PGSHIFT] = 0;          // initialse the reference count to 0
    kfree(p);
  }
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

  if((uint)v % PGSIZE || v < end || V2P(v) >= PHYSTOP)
    panic("kfree");

  if(kmem.use_lock)
    acquire(&kmem.lock);
  r = (struct run*)v;

  if(kmem.pg_refcount[V2P(v) >> PGSHIFT] > 0)         // Decrement the reference count of a page whenever someone frees it
    --kmem.pg_refcount[V2P(v) >> PGSHIFT];

  if(kmem.pg_refcount[V2P(v) >> PGSHIFT] == 0){       // Free the page only if there are no references to the page
    // Fill with junk to catch dangling refs.
    memset(v, 1, PGSIZE);
    kmem.free_pages++;                                // Increment the number of free pages by 1 when a page is freed
    r->next = kmem.freelist;
    kmem.freelist = r;

  }
  if(kmem.use_lock)
    release(&kmem.lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
char*
kalloc(void)
{
  struct run *r;

  if(kmem.use_lock)
    acquire(&kmem.lock);
  r = kmem.freelist;
  if(r){
    kmem.freelist = r->next;
    kmem.free_pages--;                                  // Decrement the number of free pages by 1 on a page allocation
    kmem.pg_refcount[V2P((char*)r) >> PGSHIFT] = 1;     // reference count of a page is set to one when it is allocated

  }
  if(kmem.use_lock)
    release(&kmem.lock);
  return (char*)r;
}


uint numFreePages(void)
{
  acquire(&kmem.lock);
  uint free_pages = kmem.free_pages;
  release(&kmem.lock);
  return free_pages;
}

void decrementReferenceCount(uint pa)
{
  if(pa < (uint)V2P(end) || pa >= PHYSTOP)
    panic("decrementReferenceCount");

  acquire(&kmem.lock);
  --kmem.pg_refcount[pa >> PGSHIFT];
  release(&kmem.lock);
}

void incrementReferenceCount(uint pa)
{
  if(pa < (uint)V2P(end) || pa >= PHYSTOP)
    panic("incrementReferenceCount");

  acquire(&kmem.lock);
  ++kmem.pg_refcount[pa >> PGSHIFT];
  release(&kmem.lock);
}
uint getReferenceCount(uint pa)
{
  if(pa < (uint)V2P(end) || pa >= PHYSTOP)
    panic("getReferenceCount");
  uint count;

  acquire(&kmem.lock);
  count = kmem.pg_refcount[pa >> PGSHIFT];
  release(&kmem.lock);

  return count;
}

