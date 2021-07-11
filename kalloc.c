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
                   // defined by the kernel linker script in kernel.ld

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  int use_lock;
  int page_cnt;
  struct run *freelist;
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
  for(; p + PGSIZE <= (char*)vend; p += PGSIZE)
    kfree(p);
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

  // Fill with junk to catch dangling refs.
  memset(v, 1, PGSIZE);

  if(kmem.use_lock)
    acquire(&kmem.lock);
  r = (struct run*)v;
  r->next = kmem.freelist;
  kmem.freelist = r;
  kmem.page_cnt++;
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
    kmem.page_cnt--;
  }
  if(kmem.use_lock)
    release(&kmem.lock);
  return (char*)r;
}

// Sanity check for free memory. Tests:
// 1. Whether the free page list contains all the
//    free memory the system should have;
// 2. The free pages are filled with 1's, as they
//    were when freed.
// Mismatched counters or errors indicate memory
// corruption!
int
kmemtest(void)
{
  int page_cnt, list_cnt;
  int page_err, err_cnt;
  struct run *r;
  char *c;

  if(kmem.use_lock)
    acquire(&kmem.lock);
  page_cnt = kmem.page_cnt; // free pages by counter
  list_cnt = 0; // free pages on linked list
  err_cnt = 0; // corrupted free pages
  for(r = kmem.freelist; r; r = r->next){
    list_cnt++;
    c = (char *)r;
    page_err = 0;
    for(int i = sizeof(void*); i < PGSIZE; i++)
      if (c[i] != 1)
        page_err = 1;
    err_cnt += page_err;
  }
  if(kmem.use_lock)
    release(&kmem.lock);

  cprintf("Free Memory Pages:\n"
    "  counter: %d\n"
    "  list:    %d\n"
    "  errors:  %d\n",
    page_cnt, list_cnt, err_cnt);

  if (page_cnt == list_cnt && !err_cnt)
    return 0;
  return -1;
}
