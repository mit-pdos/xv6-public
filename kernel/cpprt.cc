#include "types.h"
#include "kernel.hh"
#include "cpputil.hh"
#include "spinlock.h"
#include "amd64.h"

void *
operator new[](unsigned long nbytes)
{
  u64 *x = (u64*) kmalloc(nbytes + sizeof(u64), "array");
  *x = nbytes + sizeof(u64);
  return x+1;
}

void
operator delete[](void *p)
{
  u64 *x = (u64*) p;
  x--;
  kmfree(x, *x);
}

void
__cxa_pure_virtual(void)
{
  panic("__cxa_pure_virtual");
}

// Ugh: libsupc++ calls syscall(SYS_futex, ...)
extern "C" u64 syscall(void);
u64
syscall(void)
{
  return 0;
}

int
__cxa_atexit(void (*f)(void*), void *p, void *d)
{
  return 0;
}

void *__dso_handle;
std::ostream std::cout;

namespace std {

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

extern "C" void abort(void);
void
abort(void)
{
  panic("abort");
}

extern "C" void* malloc(size_t);
void*
malloc(size_t n)
{
  u64* p = (u64*) kmalloc(n+8, "cpprt malloc");
  *p = n;
  return p+1;
}

extern "C" void free(void*);
void
free(void* vp)
{
  u64* p = (u64*) vp;
  kmfree(vp, p[-1]);
}

extern "C" void* realloc(void*, size_t);
void*
realloc(void* vp, size_t n)
{
  u64* p = (u64*) vp;
  u64* np = (u64*) malloc(n);
  memcpy(np, p, p[-1]);
  free(p);
  return np;
}

extern "C" int dl_iterate_phdr(void);
int
dl_iterate_phdr(void)
{
  return -1;
}

extern "C" int __sprintf_chk(char* buf, int flags, size_t n, const char* fmt, ...);
int
__sprintf_chk(char* buf, int flags, size_t n, const char* fmt, ...)
{
  va_list ap;

  va_start(ap, fmt);
  vsnprintf(buf, n, fmt, ap);
  va_end(ap);

  return 0; // XXX
}

extern "C" void __stack_chk_fail(void);
void
__stack_chk_fail(void)
{
  panic("stack_chk_fail");
}

extern "C" char* strcpy(char*, const char*);
char*
strcpy(char* dst, const char* src)
{
  return strncpy(dst, src, __SIZE_MAX__);
}

// stdio stubs to satisfy libsupc++
int stderr = 1;

extern "C" int fputc(int c, int stream);
int
fputc(int c, int stream)
{
  cprintf("%c", (u8) c);
  return (u8) c;
}

extern "C" size_t fwrite(const void* buf, size_t n, size_t nmemb, int stream);
size_t
fwrite(const void* buf, size_t n, size_t nmemb, int stream)
{
  cprintf("%s", (char*) buf);
  return n;
}

extern "C" int fputs(const char* s, int stream);
int
fputs(const char* s, int stream)
{
  cprintf("%s", s);
  return 0;
}
