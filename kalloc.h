struct run {
  struct run *next;
};

struct kmem {
  char name[MAXNAME];
  struct spinlock lock;
  struct run *freelist;
};

extern struct kmem kmems[NCPU];
