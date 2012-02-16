#include "gc.hh"
#include "atomic.hh"
#include "crange.hh"

using std::atomic;

// A mapping of a chunk of an address space to
// a specific memory object.
enum vmatype { PRIVATE, COW};
struct vma : public rcu_freed {
  uptr va_start;               // start of mapping
  uptr va_end;                 // one past the last byte
  enum vmatype va_type;
  struct vmnode *n;
  struct spinlock lock;        // serialize fault/unmap
  char lockname[16];

  vma() : rcu_freed("vma"), va_start(0), va_end(0), va_type(PRIVATE), n(0) {
    snprintf(lockname, sizeof(lockname), "vma:%p", this);
    initlock(&lock, lockname, LOCKSTAT_VM);
  }

  ~vma();

  virtual void do_gc() { delete this; }
};

// A memory object (physical pages or inode).
struct vmnode {
  u64 npages;
  char *page[128];
  atomic<u64> ref;
  enum vmntype type;
  struct inode *ip;
  u64 offset;
  u64 sz;
};

// An address space: a set of vmas plus h/w page table.
// The elements of e[] are not ordered by address.
struct vmap {
  struct crange cr;
  struct spinlock lock;        // serialize map/lookup/unmap
  atomic<u64> ref;
  u64 alloc;
  pml4e_t *pml4;               // Page table
  char *kshared;
  char lockname[16];

  vmap();
  ~vmap();

  void decref();
  vmap* copy(int share);
  vma* lookup(uptr start, uptr len);
  int insert(vmnode *n, uptr va_start);
  int remove(uptr start, uptr len);
};
