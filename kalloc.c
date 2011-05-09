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

struct kmem kmems[NCPU];

extern char end[]; // first address after kernel loaded from ELF file

// Initialize free list of physical pages.
void
kinit(void)
{
  struct run *r;
  char *p;
  int c;
  int n;
  int i;

  for (c = 0; c < NCPU; c++) {
    kmems[c].name[0] = (char) c;
    safestrcpy(kmems[c].name+1, "kmem", MAXNAME-1);
    initlock(&kmems[c].lock, kmems[c].name);
  }

  p = (char*)PGROUNDUP((uint)end);
  n = (char*)PHYSTOP - p;
  n = n / PGSIZE;
  n = n / NCPU;
  cprintf("n = %d end = 0x%x\n", n,  p);

  for (c = 0; c < NCPU; c++) {
    for (i = 0; i < n; i++, p += PGSIZE) {
      memset(p, 1, PGSIZE);
      r = (struct run*)p;
      r->next = kmems[c].freelist;
      kmems[c].freelist = r;
    }
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

  if((uint)v % PGSIZE || v < end || (uint)v >= PHYSTOP)
    panic("kfree");

  // Fill with junk to catch dangling refs.
  memset(v, 1, PGSIZE);

  acquire(&kmem->lock);
  r = (struct run*)v;
  r->next = kmem->freelist;
  kmem->freelist = r;
  mtrace_label_register(mtrace_label_block,
			r,
			0,
			0,
			0,
			RET_EIP());

  release(&kmem->lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
char*
kalloc(void)
{
  struct run *r;

  //  cprintf("%d: kalloc 0x%x 0x%x 0x%x 0x%x 0%x\n", cpu->id, kmem, &kmems[cpu->id], kmem->freelist, PHYSTOP, kmems[1].freelist);

  acquire(&kmem->lock);
  r = kmem->freelist;
  if(r)
    kmem->freelist = r->next;
  mtrace_label_register(mtrace_label_block,
			r,
			4096,
			"kalloc",
			sizeof("kalloc"),
			RET_EIP());
  release(&kmem->lock);
  if (r == 0)
      cprintf("%d: kalloc out\n", cpunum());
  memset(r, 2, PGSIZE);
  return (char*)r;
}

