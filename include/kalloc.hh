#include "atomic.hh"

template<class T>
struct vptr {
  u128 _a;
  T ptr() const { return (T) (_a & 0xffffffffffffffffULL); }
  u64 v() const { return _a >> 64; }

  vptr(T p, u64 v) : _a((((u128)v)<<64) | (u64) p) {}
  vptr(u128 a) : _a(a) {}
};

template<class T>
class versioned {
 private:
  std::atomic<u128> _a;

 public:
  vptr<T> load() { return vptr<T>(_a.load()); }
  bool compare_exchange(const vptr<T> &expected, T desired) {
    vptr<T> n(desired, expected.v());
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
  versioned<run*> freelist;
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

