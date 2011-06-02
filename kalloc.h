struct run {
  struct run *next;
};

struct kmem {
  char name[MAXNAME];
  struct spinlock lock;
  struct run *freelist;
  uint nfree;
} __attribute__ ((aligned (CACHELINE)));

extern struct kmem kmems[NCPU];
