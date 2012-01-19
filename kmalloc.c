//
// Allocate objects smaller than a page.
//

#include "types.h"
#include "mmu.h"
#include "kernel.h"
#include "spinlock.h"
#include "kalloc.h"
#include "mtrace.h"
#include "cpu.h"

// allocate in power-of-two sizes up to 2^KMMAX
// must be < 12
#define KMMAX 11

struct header {
  struct header *next;
};

struct freelist {
  struct header *buckets[KMMAX+1];
  char name[MAXNAME];
  struct spinlock lock;
};

struct freelist freelists[NCPU];

void
kminit(void)
{
  for (int c = 0; c < NCPU; c++) {
    freelists[c].name[0] = (char) c + '0';
    safestrcpy(freelists[c].name+1, "freelist", MAXNAME-1);
    initlock(&freelists[c].lock, freelists[c].name);
  }
}

// get more space for freelists[c].buckets[b]
void
morecore(int c, int b)
{
  char *p = kalloc();
  if(p == 0)
    return;

  int sz = 1 << b;
  for(char *q = p;
      q + sz + sizeof(struct header) <= p + PGSIZE;
      q += sz + sizeof(struct header)){
    struct header *h = (struct header *) q;
    h->next = freelists[c].buckets[b];
    freelists[c].buckets[b] = h;
  }
}

void *
kmalloc(u64 nbytes)
{
  int nn = 1, b = 0;
  void *r = 0;
  struct header *h;
  int c = mycpu()->id;

  while(nn < nbytes && b <= KMMAX){
    nn *= 2;
    b++;
  }
  if(nn != (1 << b))
    panic("kmalloc oops");
  if(b > KMMAX)
    panic("kmalloc too big");

  acquire(&freelists[c].lock);
  if(freelists[c].buckets[b] == 0)
    morecore(c, b);
  h = freelists[c].buckets[b];
  if(h){
    freelists[c].buckets[b] = h->next;
    r = h + 1;
    h->next = (void *) (long) b;
  }
  release(&freelists[c].lock);

  if (r)
    mtlabel(mtrace_label_heap, r, nbytes, "kmalloc'ed", sizeof("kmalloc'ed"));
  if(r == 0)
    cprintf("kmalloc(%d) failed\n", (int) nbytes);
  return r;
}

void
kmfree(void *ap)
{
  int c = mycpu()->id;
  struct header *h;
  int b;

  acquire(&freelists[c].lock);

  h = (struct header *) ((char *)ap - sizeof(struct header));
  b = (long) h->next;
  if(b < 0 || b > KMMAX)
    panic("kmfree bad bucket");
  h->next = freelists[c].buckets[b];
  freelists[c].buckets[b] = h;

  mtunlabel(mtrace_label_heap, ap);
  release(&freelists[c].lock);
}

#if 0
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
  mtunlabel(mtrace_label_heap, ap);
  release(&freelists[mycpu()->id].lock);
}

// Caller should hold free_lock
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
    mtlabel(mtrace_label_heap, r, nbytes, "kmalloc'ed", sizeof("kmalloc'ed"));
  return r;
}

#endif

int
kmalign(void **p, int align, u64 size)
{
  void *mem = kmalloc(size + (align-1) + sizeof(void*));
  char *amem = ((char*)mem) + sizeof(void*);
  amem += align - ((uptr)amem & (align - 1));
  ((void**)amem)[-1] = mem;
  *p = amem;
  return 0;   
}

void kmalignfree(void *mem)
{
  kmfree(((void**)mem)[-1]);
}
