struct run {
  struct run *next;
};

struct kmem {
  char name[MAXNAME];
  u64 size;
  u64 ninit;
  struct spinlock lock;
  struct run *freelist;
  u64 nfree;
} __mpalign__;

extern struct kmem kmems[NCPU];
