#include "types.h"
#include "kernel.hh"
#include "cpputil.hh"
#include "spinlock.h"
#include "amd64.h"
#include "condvar.h"
#include "proc.hh"
#include "cpu.hh"
#include "elf.hh"

void*
operator new(unsigned long nbytes)
{
  panic("global operator new");

  u64* x = (u64*) kmalloc(nbytes + sizeof(u64), "cpprt new");
  *x = nbytes;
  return x+1;
}

void
operator delete(void* p)
{
  panic("global operator delete");

  u64* x = (u64*) p;
  kmfree(x-1, x[-1] + sizeof(u64));
}

void*
operator new[](unsigned long nbytes)
{
  u64* x = (u64*) kmalloc(nbytes + sizeof(u64), "array");
  *x = nbytes;
  return x+1;
}

void
operator delete[](void* p)
{
  u64* x = (u64*) p;
  kmfree(x-1, x[-1] + sizeof(u64));
}

void
__cxa_pure_virtual(void)
{
  panic("__cxa_pure_virtual");
}

int
__cxa_guard_acquire(s64 *guard)
{
  volatile u8 *x = (u8*) guard;
  volatile u32 *l = (u32*) (x+4);

  pushcli();
  while (xchg32(l, 1) != 0)
    ; /* spin */

  if (*x) {
    xchg32(l, 0);
    popcli();
    return 0;
  }
  return 1;
}

void
__cxa_guard_release(s64 *guard)
{
  volatile u8 *x = (u8*) guard;
  volatile u32 *l = (u32*) (x+4);

  *x = 1;
  __sync_synchronize();
  xchg32(l, 0);
  popcli();
}

void
__cxa_guard_abort(s64 *guard)
{
  volatile u8 *x = (u8*) guard;
  volatile u32 *l = (u32*) (x+4);

  xchg32(l, 0);
  popcli();
}

int
__cxa_atexit(void (*f)(void*), void *p, void *d)
{
  return 0;
}

extern "C" void abort(void);
void
abort(void)
{
  panic("abort");
}

static void
cxx_terminate(void)
{
  panic("cxx terminate");
}

static void
cxx_unexpected(void)
{
  panic("cxx unexpected");
}

void *__dso_handle;

namespace std {
  std::ostream cout;

  template<>
  u128
  atomic<u128>::load(memory_order __m) const
  {
    __sync_synchronize();
    u128 v = _M_i;
    __sync_synchronize();

    return v;
  }

  template<>
  bool
  atomic<u128>::compare_exchange_weak(u128 &__i1, u128 i2, memory_order __m)
  {
    // XXX no __sync_val_compare_and_swap for u128
    u128 o = __i1;
    bool ok = __sync_bool_compare_and_swap(&_M_i, o, i2);
    if (!ok)
      __i1 = _M_i;
    return ok;
  }
};

namespace __cxxabiv1 {
  void (*__terminate_handler)() = cxx_terminate;
  void (*__unexpected_handler)() = cxx_unexpected;
};

static char malloc_buf[65536];
static std::atomic<size_t> malloc_idx;

extern "C" void* malloc(size_t);
void*
malloc(size_t n)
{
  if (n > PGSIZE) {
    // libgcc_eh needs to allocate a large chunk of memory once
    size_t myoff = atomic_fetch_add(&malloc_idx, n);
    assert(myoff + n <= sizeof(malloc_buf));
    return &malloc_buf[myoff];
  }

  u64* p = (u64*) kmalloc(n+8, "cpprt malloc");
  *p = n;
  return p+1;
}

extern "C" void free(void*);
void
free(void* vp)
{
  if (vp >= malloc_buf && vp < malloc_buf + sizeof(malloc_buf))
    return;

  u64* p = (u64*) vp;
  kmfree(p-1, p[-1]+8);
}

extern "C" int dl_iterate_phdr(void);
int
dl_iterate_phdr(void)
{
  return -1;
}

extern "C" void __stack_chk_fail(void);
void
__stack_chk_fail(void)
{
  panic("stack_chk_fail");
}

extern "C" void* __cxa_get_globals(void);
void*
__cxa_get_globals(void)
{
  return myproc()->__cxa_eh_global;
}

extern "C" void* __cxa_get_globals_fast(void);
void*
__cxa_get_globals_fast(void)
{
  return myproc()->__cxa_eh_global;
}

extern "C" void __register_frame(u8*);
void
initcpprt(void)
{
  extern u8 __EH_FRAME_BEGIN__[];
  __register_frame(__EH_FRAME_BEGIN__);

  // Initialize lazy exception handling data structures
  try {
    throw 5;
  } catch (int& x) {
    assert(x == 5);
    return;
  }

  panic("no catch");
}
