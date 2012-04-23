#include "gc.hh"
#include "atomic.hh"
#include "crange_arch.hh"
#include "crange.hh"
#include "radix.hh"
#include "cpputil.hh"
#include "hwvm.hh"
#include "uwq.hh"

#define VM_CRANGE 0
#define VM_RADIX  1

struct padded_length;

using std::atomic;

// A memory object (physical pages or inode).
enum vmntype { EAGER, ONDEMAND };

struct vmnode {
  const u64 npages;
  atomic<bool> empty;
  atomic<char*> page[128];
  const enum vmntype type;
  struct inode *const ip;
  const u64 offset;
  const u64 sz;

  vmnode(u64 npg, vmntype type = EAGER,
         inode *i = 0, u64 off = 0, u64 s = 0);
  ~vmnode();
  void decref();
  void incref();
  u64 ref();
  void allocall(bool zero = true);
  int allocpg(int idx, bool zero = true);
  int loadall();
  int loadpg(off_t off);

  vmnode* copy();

  NEW_DELETE_OPS(vmnode);
private:
  atomic<u64> ref_;
};

// A mapping of a chunk of an address space to
// a specific memory object.
enum vmatype { PRIVATE, COW };

struct vma
#if VM_CRANGE
  : public range
#endif
#if VM_RADIX
  : public radix_elem
#endif
{
  const uptr vma_start;        // start of mapping
  const uptr vma_end;          // one past the last byte
  const enum vmatype va_type;
  struct vmnode * const n;

  vma(vmap *vmap, uptr start, uptr end, enum vmatype vtype, vmnode *vmn);
  ~vma();

  virtual void do_gc() { delete this; }
  NEW_DELETE_OPS(vma)
};

// An address space: a set of vmas plus h/w page table.
// The elements of e[] are not ordered by address.
struct vmap {
#if VM_CRANGE
  struct crange vmas;
#endif

#if VM_RADIX
  struct radix vmas;
#endif

  static vmap* alloc();

  atomic<u64> ref;
  pgmap *const pml4;           // Page table
  char *const kshared;

  bool replace_vma(vma *a, vma *b);

  void decref();
  void incref();

  vmap* copy(int share);
  vma* lookup(uptr start, uptr len);
  long insert(vmnode *n, uptr va_start, int dotlb);
  int remove(uptr start, uptr len);

  int pagefault(uptr va, u32 err);
  void* pagelookup(uptr va);
  int copyout(uptr va, void *p, u64 len);
  int sbrk(ssize_t n, uptr *addr);

  uptr brk_;                    // Top of heap

private:
  vmap();
  vmap(const vmap&);
  vmap& operator=(const vmap&);
  ~vmap();
  NEW_DELETE_OPS(vmap)
  int pagefault_wcow(vma *m);
  uptr unmapped_area(size_t n);

  struct spinlock brklock_;
};
