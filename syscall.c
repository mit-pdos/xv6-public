#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "x86.h"
#include "syscall.h"

// Fetch the int at addr from the current process.
int
fetchint(addr_t addr, int *ip)
{
  if(addr >= proc->sz || addr+sizeof(int) > proc->sz)
    return -1;
  *ip = *(int*)(addr);
  return 0;
}

int
fetchaddr(addr_t addr, addr_t *ip)
{
  if(addr >= proc->sz || addr+sizeof(addr_t) > proc->sz)
    return -1;
  *ip = *(addr_t*)(addr);
  return 0;
}

// Fetch the nul-terminated string at addr from the current process.
// Doesn't actually copy the string - just sets *pp to point at it.
// Returns length of string, not including nul.
int
fetchstr(addr_t addr, char **pp)
{
  char *s, *ep;

  if(addr >= proc->sz)
    return -1;
  *pp = (char*)addr;
  ep = (char*)proc->sz;
  for(s = *pp; s < ep; s++)
    if(*s == 0)
      return s - *pp;
  return -1;
}

static addr_t
fetcharg(int n)
{
  switch (n) {
  case 0: return proc->tf->rdi;
  case 1: return proc->tf->rsi;
  case 2: return proc->tf->rdx;
  case 3: return proc->tf->r10;
  case 4: return proc->tf->r8;
  case 5: return proc->tf->r9;
  }
  panic("failed fetch");
}

int
argint(int n, int *ip)
{
  *ip = fetcharg(n);
  return 0;
}

int
argaddr(int n, addr_t *ip)
{
  *ip = fetcharg(n);
  return 0;
}

// Fetch the nth word-sized system call argument as a pointer
// to a block of memory of size bytes.  Check that the pointer
// lies within the process address space.
int
argptr(int n, char **pp, int size)
{
  addr_t i;

  if(argaddr(n, &i) < 0)
    return -1;
  if(size < 0 || (uint)i >= proc->sz || (uint)i+size > proc->sz)
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
  int addr;
  if(argint(n, &addr) < 0)
    return -1;
  return fetchstr(addr, pp);
}

extern addr_t sys_chdir(void);
extern addr_t sys_close(void);
extern addr_t sys_dup(void);
extern addr_t sys_exec(void);
extern addr_t sys_exit(void);
extern addr_t sys_fork(void);
extern addr_t sys_fstat(void);
extern addr_t sys_getpid(void);
extern addr_t sys_kill(void);
extern addr_t sys_link(void);
extern addr_t sys_mkdir(void);
extern addr_t sys_mknod(void);
extern addr_t sys_open(void);
extern addr_t sys_pipe(void);
extern addr_t sys_read(void);
extern addr_t sys_sbrk(void);
extern addr_t sys_sleep(void);
extern addr_t sys_unlink(void);
extern addr_t sys_wait(void);
extern addr_t sys_write(void);
extern addr_t sys_uptime(void);

// PAGEBREAK!
static addr_t (*syscalls[])(void) = {
[SYS_fork]    sys_fork,
[SYS_exit]    sys_exit,
[SYS_wait]    sys_wait,
[SYS_pipe]    sys_pipe,
[SYS_read]    sys_read,
[SYS_kill]    sys_kill,
[SYS_exec]    sys_exec,
[SYS_fstat]   sys_fstat,
[SYS_chdir]   sys_chdir,
[SYS_dup]     sys_dup,
[SYS_getpid]  sys_getpid,
[SYS_sbrk]    sys_sbrk,
[SYS_sleep]   sys_sleep,
[SYS_uptime]  sys_uptime,
[SYS_open]    sys_open,
[SYS_write]   sys_write,
[SYS_mknod]   sys_mknod,
[SYS_unlink]  sys_unlink,
[SYS_link]    sys_link,
[SYS_mkdir]   sys_mkdir,
[SYS_close]   sys_close,
};

void
syscall(struct trapframe *tf)
{
  if (proc->killed)
    exit();
  proc->tf = tf;
  uint64 num = proc->tf->rax;
  if (num > 0 && num < NELEM(syscalls) && syscalls[num]) {
    tf->rax = syscalls[num]();
  } else {
    cprintf("%d %s: unknown sys call %d\n",
            proc->pid, proc->name, num);
    tf->rax = -1;
  }
  if (proc->killed)
    exit();
}
