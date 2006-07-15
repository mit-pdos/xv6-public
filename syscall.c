#include "types.h"
#include "param.h"
#include "mmu.h"
#include "proc.h"
#include "defs.h"
#include "x86.h"
#include "traps.h"
#include "syscall.h"
#include "spinlock.h"

/*
 * User code makes a system call with INT T_SYSCALL.
 * System call number in %eax.
 * Arguments on the stack, from the user call to the C
 * library system call function. The saved user %esp points
 * to a saved frame pointer, a program counter, and then
 * the first argument.
 *
 * Return value? Error indication? Errno?
 */

/*
 * fetch 32 bits from a user-supplied pointer.
 * returns 0 if addr was OK, -1 if illegal.
 */
int
fetchint(struct proc *p, unsigned addr, int *ip)
{
  *ip = 0;

  if(addr > p->sz - 4)
    return -1;
  memcpy(ip, p->mem + addr, 4);
  return 0;
}

int
fetcharg(int argno, int *ip)
{
  unsigned esp;

  esp = (unsigned) curproc[cpu()]->tf->tf_esp;
  return fetchint(curproc[cpu()], esp + 4 + 4*argno, ip);
}

int
putint(struct proc *p, unsigned addr, int ip)
{
  if(addr > p->sz - 4)
    return -1;
  memcpy(p->mem + addr, &ip, 4);
  return 0;
}

int
sys_pipe(void)
{
  struct fd *rfd = 0, *wfd = 0;
  int f1 = -1, f2 = -1;
  struct proc *p = curproc[cpu()];
  unsigned fdp;

  if(pipe_alloc(&rfd, &wfd) < 0)
    goto oops;
  if((f1 = fd_ualloc()) < 0)
    goto oops;
  p->fds[f1] = rfd;
  if((f2 = fd_ualloc()) < 0)
    goto oops;
  p->fds[f2] = wfd;
  if(fetcharg(0, &fdp) < 0)
    goto oops;
  if(putint(p, fdp, f1) < 0)
    goto oops;
  if(putint(p, fdp+4, f2) < 0)
    goto oops;
  return 0;

 oops:
  cprintf("sys_pipe failed\n");
  if(rfd)
    fd_close(rfd);
  if(wfd)
    fd_close(wfd);
  if(f1 >= 0)
    p->fds[f1] = 0;
  if(f2 >= 0)
    p->fds[f2] = 0;
  return -1;
}

int
sys_write(void)
{
  int fd, n, ret;
  unsigned addr;
  struct proc *p = curproc[cpu()];

  if(fetcharg(0, &fd) < 0 || fetcharg(1, &addr) < 0 || fetcharg(2, &n) < 0)
    return -1;
  if(fd < 0 || fd >= NOFILE)
    return -1;
  if(p->fds[fd] == 0)
    return -1;
  if(addr + n > p->sz)
    return -1;
  ret = fd_write(p->fds[fd], p->mem + addr, n);
  return ret;
}

int
sys_read(void)
{
  int fd, n, ret;
  unsigned addr;
  struct proc *p = curproc[cpu()];

  if(fetcharg(0, &fd) < 0 || fetcharg(1, &addr) < 0 || fetcharg(2, &n) < 0)
    return -1;
  if(fd < 0 || fd >= NOFILE)
    return -1;
  if(p->fds[fd] == 0)
    return -1;
  if(addr + n > p->sz)
    return -1;
  ret = fd_read(p->fds[fd], p->mem + addr, n);
  return ret;
}

int
sys_close(void)
{
  int fd;
  struct proc *p = curproc[cpu()];

  if(fetcharg(0, &fd) < 0)
    return -1;
  if(fd < 0 || fd >= NOFILE)
    return -1;
  if(p->fds[fd] == 0)
    return -1;
  fd_close(p->fds[fd]);
  p->fds[fd] = 0;
  return 0;
}

int
sys_fork(void)
{
  struct proc *np;

  np = newproc();
  if(np){
    np->state = RUNNABLE;
    return np->pid;
  } else {
    return -1;
  }
}

int
sys_exit(void)
{
  proc_exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return proc_wait();
}

int
sys_kill(void)
{
  int pid;

  fetcharg(0, &pid);
  return proc_kill(pid);
}

int
sys_cons_putc(void)
{
  int c;

  fetcharg(0, &c);
  cons_putc(c & 0xff);
  return 0;
}

int
sys_block(void)
{
  char buf[512];
  int i, j;
  void *c;

  cprintf("%d: call sys_block\n", cpu());
  for (i = 0; i < 100; i++) {
    if ((c = ide_start_read(i, buf, 1)) == 0) {
      panic("couldn't start read\n");
    }
    cprintf("call sleep\n");
    sleep (c, 0);
    if (ide_finish_read(c)) {
      panic("couldn't do read\n");
    }
    cprintf("sector %d: ", i);
    for (j = 0; j < 2; j++)
      cprintf("%x ", buf[j] & 0xff);
    cprintf("\n");
  }
  return 0;
}

int
sys_panic(void)
{
  struct proc *p = curproc[cpu()];
  unsigned int addr;

  fetcharg(0, &addr);
  panic(p->mem + addr);
  return 0;
}

void
syscall(void)
{
  struct proc *cp = curproc[cpu()];
  int num = cp->tf->tf_regs.reg_eax;
  int ret = -1;

  //cprintf("%x sys %d\n", cp, num);
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
  case SYS_cons_putc:
    ret = sys_cons_putc();
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
  case SYS_block:
    ret = sys_block();
    break;
  case SYS_kill:
    ret = sys_kill();
    break;
  case SYS_panic:
    ret = sys_panic();
    break;
  default:
    cprintf("unknown sys call %d\n", num);
    // XXX fault
    break;
  }
  cp->tf->tf_regs.reg_eax = ret;
}
