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

// allocate in power-of-two sizes up to 2^KMMAX (PGSIZE)
#define KMMAX 12

struct header {
  struct header *next;
};

struct freelist {
  versioned<vptr48<header*>> buckets[KMMAX+1];
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
static int
morecore(int c, int b)
{
  char *p = kalloc(nullptr);
  if(p == 0)
    return -1;

  if (ALLOC_MEMSET)
    memset(p, 3, PGSIZE);

  int sz = 1 << b;
  assert(sz >= sizeof(header));
  for(char *q = p; q + sz <= p + PGSIZE; q += sz){
    struct header *h = (struct header *) q;
    for (;;) {
      auto headptr = freelists[c].buckets[b].load();
      h->next = headptr.ptr();
      if (freelists[c].buckets[b].compare_exchange(headptr, h))
        break;
    }
  }

  return 0;
}

static int
bucket(u64 nbytes)
{
  static int bucketmap[] = {
    6,
    7,
    8,  8,
    9,  9,  9,  9,
    10, 10, 10, 10, 10, 10, 10, 10,
    11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
    12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
    12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
    12,
  };

  assert(nbytes <= PGSIZE);
  int b = bucketmap[nbytes >> 6];
  assert((1<<b) >= nbytes);
  return b;
}

void *
kmalloc(u64 nbytes, const char *name)
{
  int b = bucket(nbytes);

  struct header *h;
  int c = mycpu()->id;

  for (;;) {
    auto headptr = freelists[c].buckets[b].load();
    h = headptr.ptr();
    if (!h) {
      if (morecore(c, b) < 0) {
        cprintf("kmalloc(%d) failed\n", (int) nbytes);
        return 0;
      }
    } else {
      header *nxt = h->next;
      if (freelists[c].buckets[b].compare_exchange(headptr, nxt)) {
        if (h->next != nxt)
          panic("kmalloc: aba race");
        break;
      }
    }
  }

  mtlabel(mtrace_label_heap, (void*) h, nbytes, name, strlen(name));

  if (ALLOC_MEMSET) {
    char* chk = (char*)(h + 1);
    for (int i = 0; i < (1<<b)-sizeof(*h); i++)
      if (chk[i] != 3)
        panic("kmalloc: oops %p+%x", chk, i);
    memset(h, 4, (1<<b));
  }

  return h;
}

void
kmfree(void *ap, u64 nbytes)
{
  int b = bucket(nbytes);

  struct header *h = (struct header *) ap;
  if (ALLOC_MEMSET)
    memset(ap, 3, (1<<b));

  mtunlabel(mtrace_label_heap, ap);

  int c = mycpu()->id;
  for (;;) {
    auto headptr = freelists[c].buckets[b].load();
    h->next = headptr.ptr();
    if (freelists[c].buckets[b].compare_exchange(headptr, h))
      break;
  }
}

int
kmalign(void **p, int align, u64 size, const char *name)
{
  void *mem = kmalloc(size + (align-1) + sizeof(void*), name);
  char *amem = ((char*)mem) + sizeof(void*);
  amem += align - ((uptr)amem & (align - 1));
  ((void**)amem)[-1] = mem;
  *p = amem;
  return 0;   
}

void kmalignfree(void *mem, int align, u64 size)
{
  u64 msz = size + (align-1) + sizeof(void*);
  kmfree(((void**)mem)[-1], msz);
}
