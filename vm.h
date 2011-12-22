#define TREE

// A mapping of a chunk of an address space to
// a specific memory object.
enum vmatype { PRIVATE, COW};
struct vma {
  uptr va_start;               // start of mapping
  uptr va_end;                 // one past the last byte
  enum vmatype va_type;
  struct vmnode *n;
  struct spinlock lock;        // serialize fault/unmap
  char lockname[16];
};

// A memory object (physical pages or inode).
enum vmntype { EAGER, ONDEMAND};  
struct vmnode {
  u64 npages;
  char *page[128];
  u64 ref;
  enum vmntype type;
  struct inode *ip;
  u64 offset;
  u64 sz;
};

// An address space: a set of vmas plus h/w page table.
// The elements of e[] are not ordered by address.
struct vmap {
#ifdef TREE
  // struct node* root;
  struct crange* cr;
#else
  struct vma* e[16];
#endif
  struct spinlock lock;        // serialize map/lookup/unmap
  u64 ref;
  u64 alloc;
  pml4e_t *pml4;               // Page table
  char lockname[16];
};
