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
#include "fd.h"
#include "fcntl.h"

// User code makes a system call with INT T_SYSCALL.
// System call number in %eax.
// Arguments on the stack, from the user call to the C
// library system call function. The saved user %esp points
// to a saved program counter, and then the first argument.

// Fetch 32 bits from a user-supplied pointer.
// Returns 0 if addr was OK, -1 if illegal.
int
fetchint(struct proc *p, uint addr, int *ip)
{
  *ip = 0;

  if(addr > p->sz - 4)
    return -1;
  *ip = *(int*)(p->mem + addr);
  return 0;
}

// Fetch byte from a user-supplied pointer.
// Returns 0 on success, -1 if pointer is illegal.
int
fetchbyte(struct proc *p, uint addr, char *c)
{
  if(addr >= p->sz)
    return -1;
  *c = *(p->mem + addr);
  return 0;
}

int
fetcharg(int argno, void *ip)
{
  uint esp;

  esp = (uint) curproc[cpu()]->tf->esp;
  return fetchint(curproc[cpu()], esp + 4 + 4*argno, ip);
}

// Check that an entire string is valid in user space.
// Returns the length, not including null, or -1.
int
checkstring(uint s)
{
  char c;
  int len = 0;

  while(1){
    if(fetchbyte(curproc[cpu()], s, &c) < 0)
      return -1;
    if(c == '\0')
      return len;
    len++;
    s++;
  }
}

int
putint(struct proc *p, uint addr, int x)
{
  if(addr > p->sz - 4)
    return -1;
  memmove(p->mem + addr, &x, 4);
  return 0;
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
    // XXX fault
    break;
  }
  cp->tf->eax = ret;
}
