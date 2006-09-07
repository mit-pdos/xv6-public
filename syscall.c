#include "types.h"
#include "stat.h"
#include "param.h"
#include "mmu.h"
#include "proc.h"
#include "defs.h"
#include "x86.h"
#include "traps.h"
#include "syscall.h"
#include "spinlock.h"
#include "buf.h"
#include "fs.h"
#include "fsvar.h"
#include "elf.h"
#include "file.h"
#include "fcntl.h"

// User code makes a system call with INT T_SYSCALL.
// System call number in %eax.
// Arguments on the stack, from the user call to the C
// library system call function. The saved user %esp points
// to a saved program counter, and then the first argument.

// Fetch the int at addr from process p.
int
fetchint(struct proc *p, uint addr, int *ip)
{
  if(addr >= p->sz || addr+4 > p->sz)
    return -1;
  *ip = *(int*)(p->mem + addr);
  return 0;
}

// Fetch the nul-terminated string at addr from process p.
// Doesn't actually copy the string - just sets *pp to point at it.
// Returns length of string, not including nul.
int
fetchstr(struct proc *p, uint addr, char **pp)
{
  char *cp, *ep;

  if(addr >= p->sz)
    return -1;
  *pp = p->mem + addr;
  ep = p->mem + p->sz;
  for(cp = *pp; cp < ep; cp++)
    if(*cp == 0)
      return cp - *pp;
  return -1;
}

// Fetch the argno'th word-sized system call argument as an integer.
int
argint(int argno, int *ip)
{
  struct proc *p = curproc[cpu()];

  return fetchint(p, p->tf->esp + 4 + 4*argno, ip);
}

// Fetch the nth word-sized system call argument as a pointer
// to a block of memory of size n bytes.  Check that the pointer
// lies within the process address space.
int
argptr(int argno, char **pp, int size)
{
  int i;
  struct proc *p = curproc[cpu()];
  
  if(argint(argno, &i) < 0)
    return -1;
  if((uint)i >= p->sz || (uint)i+size >= p->sz)
    return -1;
  *pp = p->mem + i;
  return 0;
}

// Fetch the nth word-sized system call argument as a string pointer.
// Check that the pointer is valid and the string is nul-terminated.
// (There is no shared writable memory, so the string can't change
// between this check and being used by the kernel.)
int
argstr(int argno, char **pp)
{
  int addr;
  if(argint(argno, &addr) < 0)
    return -1;
  return fetchstr(curproc[cpu()], addr, pp);
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
extern int sys_unlink(void);
extern int sys_wait(void);
extern int sys_write(void);

void
syscall(void)
{
  struct proc *cp = curproc[cpu()];
  int num = cp->tf->eax;
  int ret = -1;

  switch(num){
  case SYS_fork:
    ret = sys_fork();
    break;
  case SYS_exit:
    ret = sys_exit();
    break;
  case SYS_wait:
    ret = sys_wait();
    break;
  case SYS_pipe:
    ret = sys_pipe();
    break;
  case SYS_write:
    ret = sys_write();
    break;
  case SYS_read:
    ret = sys_read();
    break;
  case SYS_close:
    ret = sys_close();
    break;
  case SYS_kill:
    ret = sys_kill();
    break;
  case SYS_exec:
    ret = sys_exec();
    break;
  case SYS_open:
    ret = sys_open();
    break;
  case SYS_mknod:
    ret = sys_mknod();
    break;
  case SYS_unlink:
    ret = sys_unlink();
    break;
  case SYS_fstat:
    ret = sys_fstat();
    break;
  case SYS_link:
    ret = sys_link();
    break;
  case SYS_mkdir:
    ret = sys_mkdir();
    break;
  case SYS_chdir:
    ret = sys_chdir();
    break;
  case SYS_dup:
    ret = sys_dup();
    break;
  case SYS_getpid:
    ret = sys_getpid();
    break;
  case SYS_sbrk:
    ret = sys_sbrk();
    break;
  default:
    cprintf("unknown sys call %d\n", num);
    // Maybe kill the process?
    break;
  }
  cp->tf->eax = ret;
}
