#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "kernel.h"
#include "spinlock.h"
#include "condvar.h"
#include "queue.h"
#include "proc.h"
#include "x86.h"
#include "syscall.h"
#include "xv6-mtrace.h"
#include "cpu.h"

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
  return fetchint64(myproc()->tf->rsp + 8 + 8*n, ip);
}

int
argint32(int n, int *ip)
{
  int r;
  u64 i;

  r = fetchint64(myproc()->tf->rsp + 8 + 8*n, &i);
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

extern int sys_chdir(void);
extern int sys_close(void);
extern int sys_dup(void);
extern int sys_exec(void);
extern int sys_exit(void);
extern int sys_fork(void);
extern int sys_fstat(void);
extern int sys_getpid(void);
extern int sys_kill(void);
extern int sys_link(void);
extern int sys_mkdir(void);
extern int sys_mknod(void);
extern int sys_open(void);
extern int sys_pipe(void);
extern int sys_read(void);
extern int sys_sbrk(void);
extern int sys_sleep(void);
extern int sys_unlink(void);
extern int sys_wait(void);
extern int sys_write(void);
extern int sys_uptime(void);
extern int sys_map(void);
extern int sys_unmap(void);
extern int sys_halt(void);

static int (*syscalls[])(void) = {
  [SYS_chdir] =  sys_chdir,
  [SYS_close] =  sys_close,
  [SYS_dup] =    sys_dup,
  [SYS_exec] =   sys_exec,
  [SYS_exit] =   sys_exit,
  [SYS_fork] =   sys_fork,
  [SYS_fstat] =  sys_fstat,
  [SYS_getpid] = sys_getpid,
  [SYS_kill] =   sys_kill,
  [SYS_link] =   sys_link,
  [SYS_mkdir] =  sys_mkdir,
  [SYS_mknod] =  sys_mknod,
  [SYS_open] =   sys_open,
  [SYS_pipe] =   sys_pipe,
  [SYS_read] =   sys_read,
  [SYS_sbrk] =   sys_sbrk,
  [SYS_sleep] =  sys_sleep,
  [SYS_unlink] = sys_unlink,
  [SYS_wait] =   sys_wait,
  [SYS_write] =  sys_write,
  [SYS_uptime] = sys_uptime,
  [SYS_map] =    sys_map,
  [SYS_unmap] =  sys_unmap,
  [SYS_halt] =   sys_halt,
};

void
syscall(void)
{
  int num;

  num = myproc()->tf->rax;
  if(num >= 0 && num < NELEM(syscalls) && syscalls[num]) {
    mtrace_kstack_start(syscalls[num], proc);
    mtrace_call_set(1, cpunum());
    myproc()->tf->rax = syscalls[num]();
    mtrace_kstack_stop(myproc());
    mtrace_call_set(0, cpunum());
  } else {
    cprintf("%d %s: unknown sys call %d\n",
            myproc()->pid, myproc()->name, num);
    myproc()->tf->rax = -1;
  }
}
