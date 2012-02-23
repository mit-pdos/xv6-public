#include "atomic.hh"

struct run {
  struct run *next;
};

struct kmem {
  char name[MAXNAME];
  u64 size;
  u64 ninit;
  std::atomic<run*> freelist;
  std::atomic<u64> nfree;
} __mpalign__;

enum {
  slab_stack,
  slab_perf,
  slab_kshared,
  slab_type_max
};

extern struct kmem kmems[NCPU];
extern struct kmem slabmem[slab_type_max][NCPU];

