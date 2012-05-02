#include "gc.hh"
#include "atomic.hh"
#include "crange_arch.hh"
#include "crange.hh"
#include "radix.hh"
#include "cpputil.hh"
#include "hwvm.hh"
#include "uwq.hh"
#include "distref.hh"

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
  struct inode *const ip;
};

// A mapping of a chunk of an address space to
// a specific memory object.
enum vmatype { PRIVATE, COW };

struct vma
#if VM_CRANGE
  : public range
#endif
#if VM_RADIX
  : public radix_elem, public distributed_refcnt
#endif
{
  const uptr vma_start;        // start of mapping
  const uptr vma_end;          // one past the last byte
  const enum vmatype va_type;
  struct vmnode * const n;

  vma(vmap *vmap, uptr start, uptr end, enum vmatype vtype, vmnode *vmn);
  ~vma();

  NEW_DELETE_OPS(vma);

  virtual void do_gc() { delete this; }

#if VM_RADIX
  void decref(u64 delta)
  {
    distref_dec(delta);
  }

  void incref(u64 delta)
  {
    distref_inc(delta);
  }

private:
  void distref_free()
  {
    gc_delayed(this);
  }
#endif
};

class print_stream;
void to_stream(print_stream *s, vma *v);

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
  char *const kshared;

  bool replace_vma(vma *a, vma *b);

  void decref();
  void incref();

  vmap* copy(int share, proc_pgmap* pgmap);
  vma* lookup(uptr start, uptr len);
  long insert(vmnode *n, uptr va_start, int dotlb, proc_pgmap* pgmap);
  int remove(uptr start, uptr len, proc_pgmap* pgmap);

  int pagefault(uptr va, u32 err, proc_pgmap* pgmap);
  void* pagelookup(uptr va);
  int copyout(uptr va, void *p, u64 len);
  int sbrk(ssize_t n, uptr *addr);

  void add_pgmap(proc_pgmap* pgmap);
  void rem_pgmap(proc_pgmap* pgmap);

  uptr brk_;                    // Top of heap

private:
  vmap();
  vmap(const vmap&);
  vmap& operator=(const vmap&);
  ~vmap();
  NEW_DELETE_OPS(vmap)
  int pagefault_wcow(vma *m, proc_pgmap* pgmap);
  uptr unmapped_area(size_t n);

  struct spinlock brklock_;

  // XXX(sbw) most likely an awful hash function
  static u64 proc_pgmap_hash(proc_pgmap* const & p)
  {
    return (u64) p;
  }
  xns<proc_pgmap*, proc_pgmap*, proc_pgmap_hash> pgmap_list_;
};
