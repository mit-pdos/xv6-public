// Physical memory allocator, intended to allocate
// memory for user processes, kernel stacks, page table pages,
// and pipe buffers. Allocates 4096-byte pages.

#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "spinlock.h"
#include "proc.h"
#include "rammap.h"
#include "x86.h"

void freerange(void *vstart, void *vend);
extern char end[]; // first address after kernel loaded from ELF file
                   // defined by the kernel linker script in kernel.ld

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  int use_lock;
  struct run *freelist;
} kmem;

struct {
  struct spinlock lock;
  int use_lock;
  uint map[RAMMAP_PAGES];
} rammap;

void
rammap_make_entry(uint pa, uint va, uint pid, uint swap)
{
  // Brand: Cooper, what are you doing??
  // Cooper: Locking.
  if(rammap.use_lock)
    acquire(&rammap.lock);
  rammap.map[PA_2_RDX(pa)] = PACK(va, pid, swap);
  if(rammap.use_lock)
    release(&rammap.lock);
}

// Initialization happens in two phases.
// 1. main() calls kinit1() while still using entrypgdir to place just
// the pages mapped by entrypgdir on free list.
// 2. main() calls kinit2() with the rest of the physical pages
// after installing a full page table that maps them on all cores.
void
kinit1(void *vstart, void *vend)
{
  initlock(&kmem.lock, "kmem");
  initlock(&rammap.lock, "rammap");
  kmem.use_lock = 0;
  rammap.use_lock = 0;
  freerange(vstart, vend);
}

void
kinit2(void *vstart, void *vend)
{
  freerange(vstart, vend);
  kmem.use_lock = 1;
  rammap.use_lock = 1;
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
  if(kmem.use_lock)
    release(&kmem.lock);

  // Mark page as free in rammap.
  if(rammap.use_lock)
    acquire(&rammap.lock);
  rammap.map[PA_2_RDX(V2P((uint)v))] = 0;
  if(rammap.use_lock)
    release(&rammap.lock);
}

// Returns the PA of the evicted page in RAM
static uint
evict_page(void)
{
  uint pa, va, pid;
  uint victim_entry = 0;
  int victim_idx = -1;
  struct proc* vproc;

  // Choose a victim to evict
  acquire(&rammap.lock);
  for(int i = PA_2_RDX(V2P(PGROUNDUP((uint)end))); i < RAMMAP_PAGES; i++) {
    if(SWAP_PERM(rammap.map[i])) {
      victim_entry = rammap.map[i];
      victim_idx = i;
      break;
    }
  }
  release(&rammap.lock);

  // No swappable pages found
  if(victim_idx < 0)
    return 0;

  // Find out the owner pid and the virtual
  // address(vpn) which this page is mapped to.
  pid = GET_PID(victim_entry);
  va = PTE_ADDR(victim_entry);
  pa = RDX_2_PA(victim_idx);

  // Fetch the struct proc of owner process
  if((vproc = fetch_proc(pid)) == 0)
    panic("corrupt rammap");

  // Write contents of the page to swapspace
  // record the swapslot index where the page
  // was written.
  uint slot = write_to_swap(P2V(pa));

  // Store the swapslot index into the PTE
  // corresponding to the evicted page in
  // owner process's address space.
  // Set PTE_P bit to zero.
  pte_t *pte = walkpgdir(vproc->pgdir, (char*)va, 0);
  if(!pte)
    panic("corrupt rammap");
  *pte = ((slot << 12) | (PTE_FLAGS(*pte) & ~PTE_P));

  // Invalidate corresponding TLB entry.
  invlpg((void*)va);

  // Finally update corresponding rammap entry
  // of the page as free.
  acquire(&rammap.lock);
  rammap.map[victim_idx] = 0;
  release(&rammap.lock);

  // Return the physical address of the now free page.
  return pa;
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
char*
kalloc()
{
  struct run *r;

  if(kmem.use_lock)
    acquire(&kmem.lock);
  r = kmem.freelist;
  if(r) // fixed.
    kmem.freelist = r->next;
  if(kmem.use_lock)
    release(&kmem.lock);

  // No free pages found
  if(!r) {
    // Choose and evict a page from RAM
    // to swapspace and get the PA of the
    // now free RAM frame.
    uint pa = evict_page();

    // Can't evict any page from RAM
    // kalloc() fails as normal here.
    if(!pa)
      return 0;
    r = (struct run*)P2V((char*)pa);

    // Clear the page before returning.
    memset((char*)r, 0, PGSIZE);
  }

  // mark page to be owned by kernel initially
  if(r) {
    if(rammap.use_lock)
      acquire(&rammap.lock);
    rammap.map[PA_2_RDX(V2P(r))] = PACK(0, PID_KERNEL, 0);
    if(rammap.use_lock)
      release(&rammap.lock);
  }

  return (char*)r;
}
