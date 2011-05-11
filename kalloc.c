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

static void __attribute__((unused))
kmemprint(void)
{
  cprintf("free pages: [ ");
  for (uint i = 0; i < NCPU; i++)
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
  kmem->nfree++;
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
  struct run *r = 0;

  //  cprintf("%d: kalloc 0x%x 0x%x 0x%x 0x%x 0%x\n", cpu->id, kmem, &kmems[cpu->id], kmem->freelist, PHYSTOP, kmems[1].freelist);

  for (uint i = 0; r == 0 && i < NCPU; i++) {
    struct kmem *m = &kmems[(i + cpunum()) % NCPU];
    acquire(&m->lock);
    r = m->freelist;
    if (r) {
      m->freelist = r->next;
      m->nfree--;
    }
    release(&m->lock);
  }

  mtrace_label_register(mtrace_label_block,
			r,
			4096,
			"kalloc",
			sizeof("kalloc"),
			RET_EIP());

  if (r == 0) {
    kmemprint();
    panic("out of memory");
  }
  memset(r, 2, PGSIZE);
  return (char*)r;
}

