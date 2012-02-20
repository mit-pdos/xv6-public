#include "gc.hh"
#include "atomic.hh"
#include "crange.hh"
#include "cpputil.hh"
#include "hwvm.hh"

using std::atomic;

// A memory object (physical pages or inode).
enum vmntype { EAGER, ONDEMAND };

struct vmnode {
  u64 npages;
  char *page[128];
  atomic<u64> ref;
  enum vmntype type;
  struct inode *ip;
  u64 offset;
  u64 sz;

  vmnode(u64 npg, vmntype type = EAGER);
  ~vmnode();
  void decref();
  int allocpg();
  vmnode* copy();

  int load(inode *ip, u64 offset, u64 sz);
  int demand_load();
};

// A mapping of a chunk of an address space to
// a specific memory object.
enum vmatype { PRIVATE, COW };

struct vma : public range {
  const uptr vma_start;        // start of mapping
  const uptr vma_end;          // one past the last byte
  const enum vmatype va_type;
  struct vmnode * const n;

  vma(vmap *vmap, uptr start, uptr end, enum vmatype vtype, vmnode *vmn);
  ~vma();

  virtual void do_gc() { delete this; }
};

// An address space: a set of vmas plus h/w page table.
// The elements of e[] are not ordered by address.
struct vmap {
  struct crange cr;
  struct spinlock lock;        // serialize map/lookup/unmap
  atomic<u64> ref;
  u64 alloc;
  pgmap *const pml4;           // Page table
  char *const kshared;
  char lockname[16];

  vmap();
  ~vmap();

  bool replace_vma(vma *a, vma *b);

  void decref();
  vmap* copy(int share);
  vma* lookup(uptr start, uptr len);
  int insert(vmnode *n, uptr va_start);
  int remove(uptr start, uptr len);

  int pagefault(uptr va, u32 err);
  int copyout(uptr va, void *p, u64 len);

 private:
  int pagefault_wcow(vma *m);
};
