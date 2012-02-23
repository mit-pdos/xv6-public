//
// Allocate objects smaller than a page.
//

#include "types.h"
#include "mmu.h"
#include "kernel.hh"
#include "spinlock.h"
#include "kalloc.hh"
#include "mtrace.h"
#include "cpu.hh"

// allocate in power-of-two sizes up to 2^KMMAX
// must be < 12
#define KMMAX 11

struct header {
  struct header *next;
};

struct freelist {
  std::atomic<header*> buckets[KMMAX+1];
  char name[MAXNAME];
};

struct freelist freelists[NCPU];

void
kminit(void)
{
  for (int c = 0; c < NCPU; c++) {
    freelists[c].name[0] = (char) c + '0';
    safestrcpy(freelists[c].name+1, "freelist", MAXNAME-1);
  }
}

// get more space for freelists[c].buckets[b]
int
morecore(int c, int b)
{
  char *p = kalloc();
  if(p == 0)
    return -1;

  int sz = 1 << b;
  for(char *q = p;
      q + sz + sizeof(struct header) <= p + PGSIZE;
      q += sz + sizeof(struct header)){
    struct header *h = (struct header *) q;
    h->next = freelists[c].buckets[b];
    freelists[c].buckets[b] = h;
  }

  return 0;
}

void *
kmalloc(u64 nbytes)
{
  int nn = 1, b = 0;

  while(nn < nbytes && b <= KMMAX){
    nn *= 2;
    b++;
  }
  if(nn != (1 << b))
    panic("kmalloc oops");
  if(b > KMMAX)
    panic("kmalloc too big");

  scoped_gc_epoch gc;
  struct header *h;
  int c = mycpu()->id;

  for (;;) {
    h = freelists[c].buckets[b];
    if (!h) {
      if (morecore(c, b) < 0) {
        cprintf("kmalloc(%d) failed\n", (int) nbytes);
        return 0;
      }
    } else {
      if (cmpxch(&freelists[c].buckets[b], h, h->next))
        break;
    }
  }

  void *r = h + 1;
  h->next = (header*) (long) b;

  mtlabel(mtrace_label_heap, r, nbytes, "kmalloc'ed", sizeof("kmalloc'ed"));
  return r;
}

void
kmfree(void *ap)
{
  int c = mycpu()->id;
  struct header *h;
  int b;

  h = (struct header *) ((char *)ap - sizeof(struct header));
  b = (long) h->next;
  if(b < 0 || b > KMMAX)
    panic("kmfree bad bucket");

  verifyfree((char*) ap, (1<<b) - sizeof(struct header));
  if (ALLOC_MEMSET)
    memset(ap, 3, (1<<b) - sizeof(struct header));

  h->next = freelists[c].buckets[b];
  while (!cmpxch_update(&freelists[c].buckets[b], &h->next, h))
    ; /* spin */

  mtunlabel(mtrace_label_heap, ap);
}

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
