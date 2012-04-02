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

extern "C" int __uaccess_mem(void* dst, const void* src, u64 size);
extern "C" int __uaccess_str(char* dst, const char* src, u64 size);
extern "C" int __uaccess_int64(uptr addr, u64* ip);

int
fetchmem(void* dst, const void* usrc, u64 size)
{
  if(mycpu()->ncli != 0)
    panic("fetchstr: cli'd");
  return __uaccess_mem(dst, usrc, size);
}

int
putmem(void *udst, const void *src, u64 size)
{
  if(mycpu()->ncli != 0)
    panic("fetchstr: cli'd");
  return __uaccess_mem(udst, src, size);
}

int
fetchstr(char* dst, const char* usrc, u64 size)
{
  if(mycpu()->ncli != 0)
    panic("fetchstr: cli'd");
  return __uaccess_str(dst, usrc, size);
}

int
fetchint64(uptr addr, u64 *ip)
{
  if(mycpu()->ncli != 0)
    panic("fetchstr: cli'd");
  return __uaccess_int64(addr, ip);
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

u64
syscall(u64 a0, u64 a1, u64 a2, u64 a3, u64 a4, u64 num)
{
  mt_ascope ascope("syscall(%lx,%lx,%lx,%lx,%lx,%lx)", num, a0, a1, a2, a3, a4);

  for (;;) {
    try {
      if(num < SYS_ncount && syscalls[num]) {
        mtstart(syscalls[num], myproc());
        mtrec();
        u64 r = syscalls[num](a0, a1, a2, a3, a4);
        mtstop(myproc());
        mtign();
        return r;
      } else {
        cprintf("%d %s: unknown sys call %ld\n",
                myproc()->pid, myproc()->name, num);
        return -1;
      }
    } catch (retryable& e) {
      cprintf("%d: syscall retry\n", myproc()->pid);
      yield();
    }
  }
}
