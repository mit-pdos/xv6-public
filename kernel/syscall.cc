#include "types.h"
#include "mmu.h"
#include "kernel.hh"
#include "spinlock.h"
#include "condvar.h"
#include "queue.h"
#include "proc.hh"
#include "amd64.h"
#include "syscall.h"
#include "cpu.hh"
#include "kmtrace.hh"

extern "C" int __fetchstr(char* dst, const char* usrc, unsigned size);
extern "C" int __fetchint64(uptr addr, u64* ip);

int
fetchstr(char* dst, const char* usrc, unsigned size)
{
  if(mycpu()->ncli != 0)
    panic("fetchstr: cli'd");
  return __fetchstr(dst, usrc, size);
}

int
fetchint64(uptr addr, u64 *ip)
{
  if(mycpu()->ncli != 0)
    panic("fetchstr: cli'd");
  return __fetchint64(addr, ip);
}

// Fetch the nul-terminated string at addr from process p.
// Doesn't actually copy the string - just sets *pp to point at it.
// Returns length of string, not including nul.
int
argcheckstr(const char *addr)
{
  const char *s = addr;

  while(1){
    if(pagefault(myproc()->vmap, (uptr) s, 0) < 0)
      return -1;
    if(*s == 0)
      return s - addr;
    s++;
  }
  return -1;
}

// Fetch the nth word-sized system call argument as a pointer
// to a block of memory of size n bytes.  Check that the pointer
// lies within the process address space.
int
argcheckptr(void *p, int size)
{
  u64 i = (u64) p;
  for(uptr va = PGROUNDDOWN(i); va < i+size; va = va + PGSIZE)
    if(pagefault(myproc()->vmap, va, 0) < 0)
      return -1;
  return 0;
}

static int
umemptr(void *umem, void **ret, u64 size)
{
  uptr ptr = (uptr) umem;

  for(uptr va = PGROUNDDOWN(ptr); va < ptr+size; va = va + PGSIZE)
    if(pagefault(myproc()->vmap, va, 0) < 0)
      return -1;

  *ret = umem;
  return 0;
}

int
umemcpy(void *dst, void *umem, u64 size)
{
  void *ptr;

  if (umemptr(umem, &ptr, size))
    return -1;

  memmove(dst, ptr, size);
  return 0;
}

int
kmemcpy(void *umem, void *src, u64 size)
{
  void *ptr;

  if (umemptr(umem, &ptr, size))
    return -1;

  memmove(ptr, src, size);
  return 0;
}

u64
syscall(u64 a0, u64 a1, u64 a2, u64 a3, u64 a4, u64 num)
{
  u64 r;

  mt_ascope ascope("syscall(%lx,%lx,%lx,%lx,%lx,%lx)", num, a0, a1, a2, a3, a4);
  if(num < SYS_ncount && syscalls[num]) {
    mtstart(syscalls[num], myproc());
    mtrec();
    r = syscalls[num](a0, a1, a2, a3, a4);
    mtstop(myproc());
    mtign();
  } else {
    cprintf("%d %s: unknown sys call %ld\n",
            myproc()->pid, myproc()->name, num);
    r = -1;
  }

  return r;
}
