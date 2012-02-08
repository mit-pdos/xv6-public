#include "types.h"
#include "mmu.h"
#include "kernel.h"
#include "spinlock.h"
#include "condvar.h"
#include "queue.h"
#include "proc.h"
#include "amd64.h"
#include "syscall.h"
#include "cpu.h"
#include "kmtrace.h"

// User code makes a system call with INT T_SYSCALL.
// System call number in %eax.
// Arguments on the stack, from the user call to the C
// library system call function. The saved user %esp points
// to a saved program counter, and then the first argument.

// Fetch the int at addr from process p.
int
fetchint64(uptr addr, u64 *ip)
{
  if(pagefault(myproc()->vmap, addr, 0) < 0)
    return -1;
  if(pagefault(myproc()->vmap, addr+sizeof(*ip)-1, 0) < 0)
    return -1;
  *ip = *(u64*)(addr);
  return 0;
}

// Fetch the nul-terminated string at addr from process p.
// Doesn't actually copy the string - just sets *pp to point at it.
// Returns length of string, not including nul.
int
fetchstr(uptr addr, char **pp)
{
  char *s = (char *) addr;

  while(1){
    if(pagefault(myproc()->vmap, (uptr) s, 0) < 0)
      return -1;
    if(*s == 0){
      *pp = (char*)addr;
      return s - *pp;
    }
    s++;
  }
  return -1;
}

// Fetch the nth 64-bit system call argument.
int
argint64(int n, u64 *ip)
{
  switch(n) {
  case 0: *ip = myproc()->tf->rdi; break;
  case 1: *ip = myproc()->tf->rsi; break;
  case 2: *ip = myproc()->tf->rdx; break;
  case 3: *ip = myproc()->tf->rcx; break;
  case 4: *ip = myproc()->tf->r8;  break;
  case 5: *ip = myproc()->tf->r9;  break;
  default:
    cprintf("argint64: bad arg %d\n", n);
    return -1;
  }

  return 0;
}

int
argint32(int n, int *ip)
{
  int r;
  u64 i;

  r = argint64(n, &i);
  if (r >= 0)
    *ip = i;
  return r;
}

// Fetch the nth word-sized system call argument as a pointer
// to a block of memory of size n bytes.  Check that the pointer
// lies within the process address space.
int
argptr(int n, char **pp, int size)
{
  u64 i;
  
  if(argint64(n, &i) < 0)
    return -1;
  for(uptr va = PGROUNDDOWN(i); va < i+size; va = va + PGSIZE)
    if(pagefault(myproc()->vmap, va, 0) < 0)
      return -1;
  *pp = (char*)i;
  return 0;
}

// Fetch the nth word-sized system call argument as a string pointer.
// Check that the pointer is valid and the string is nul-terminated.
// (There is no shared writable memory, so the string can't change
// between this check and being used by the kernel.)
int
argstr(int n, char **pp)
{
  uptr addr;
  if(argint64(n, &addr) < 0)
    return -1;
  return fetchstr(addr, pp);
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

extern long sys_chdir(void);
extern long sys_close(void);
extern long sys_dup(void);
extern long sys_exec(void);
extern long sys_exit(void);
extern long sys_fork(void);
extern long sys_fstat(void);
extern long sys_getpid(void);
extern long sys_kill(void);
extern long sys_link(void);
extern long sys_mkdir(void);
extern long sys_mknod(void);
extern long sys_open(void);
extern long sys_pipe(void);
extern long sys_read(void);
extern long sys_sbrk(void);
extern long sys_sleep(void);
extern long sys_unlink(void);
extern long sys_wait(void);
extern long sys_write(void);
extern long sys_uptime(void);
extern long sys_map(void);
extern long sys_unmap(void);
extern long sys_halt(void);
extern long sys_socket(int, int, int);
extern long sys_bind(int, void*, int);
extern long sys_listen(int, int);
extern long sys_accept(int, void*, void*);
extern long sys_pread(int fd, void *ubuf, size_t count, off_t offset);

#define SYSCALL(name) [SYS_##name] = (void*)sys_##name

static long (*syscalls[])(u64, u64, u64, u64, u64, u64) = {
  SYSCALL(chdir),
  SYSCALL(close),
  SYSCALL(dup),
  SYSCALL(exec),
  SYSCALL(exit),
  SYSCALL(fork),
  SYSCALL(fstat),
  SYSCALL(getpid),
  SYSCALL(kill),
  SYSCALL(link),
  SYSCALL(mkdir),
  SYSCALL(mknod),
  SYSCALL(open),
  SYSCALL(pipe),
  SYSCALL(read),
  SYSCALL(sbrk),
  SYSCALL(sleep),
  SYSCALL(unlink),
  SYSCALL(wait),
  SYSCALL(write),
  SYSCALL(uptime),
  SYSCALL(map),
  SYSCALL(unmap),
  SYSCALL(halt),
  SYSCALL(socket),
  SYSCALL(bind),
  SYSCALL(listen),
  SYSCALL(accept),
  SYSCALL(pread),
};

void
syscall(void)
{
  struct trapframe *tf;
  int num;

  tf = myproc()->tf;
  num = tf->rax;
  if(num >= 0 && num < NELEM(syscalls) && syscalls[num]) {
    mtstart(syscalls[num], myproc());
    mtrec();
    tf->rax = syscalls[num](tf->rdi, tf->rsi, tf->rdx, 
                            tf->rcx, tf->r8, tf->r9);
    mtstop(myproc());
    mtign();
  } else {
    cprintf("%d %s: unknown sys call %d\n",
            myproc()->pid, myproc()->name, num);
    tf->rax = -1;
  }
}
