#include "atomic.hh"

template<class T>
struct vptr64 {
  typedef u128 __inttype;
  typedef T __ptrtype;
  __inttype _a;

  T ptr() const { return (T) iptr(); }
  u64 iptr() const { return _a & 0xffffffffffffffffULL; }
  u64 v() const { return _a >> 64; }

  vptr64(T p, u64 v) : _a((((u128)v)<<64) | (u64) p) {}
  vptr64(u128 a) : _a(a) {}
};

template<class T>
struct vptr48 {
  typedef u64 __inttype;
  typedef T __ptrtype;
  __inttype _a;

  T ptr() const {
    u64 i = iptr();
    if (i & (1ULL << 47))
      i += 0xffffULL << 48;
    return (T) i;
  }
  u64 iptr() const { return _a & 0xffffffffffffULL; }
  u16 v() const { return _a >> 48; }

  vptr48(T p, u16 v) : _a((((u64)v)<<48) | (((u64)p) & 0xffffffffffffULL)) {}
  vptr48(u64 a) : _a(a) {}
};

template<class VPTR>
class versioned {
 private:
  std::atomic<typename VPTR::__inttype> _a;

 public:
  VPTR load() { return VPTR(_a.load()); }
  bool compare_exchange(const VPTR &expected, typename VPTR::__ptrtype desired) {
    VPTR n(desired, expected.v());
    return cmpxch(&_a, expected._a, n._a);
  }
};

struct run {
  struct run *next;
};

struct kmem {
  char name[MAXNAME];
  u64 size;
  u64 ninit;
  versioned<vptr48<run*>> freelist;
  std::atomic<u64> nfree;

  run* alloc(const char* name);
  void free(run* r);
} __mpalign__;

enum {
  slab_stack,
  slab_perf,
  slab_kshared,
  slab_wq,
  slab_userwq,
  slab_type_max
};

extern struct kmem kmems[NCPU];
extern struct kmem slabmem[slab_type_max][NCPU];
