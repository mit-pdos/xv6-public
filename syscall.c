#include "types.h"
#include "param.h"
#include "mmu.h"
#include "proc.h"
#include "defs.h"
#include "x86.h"
#include "traps.h"
#include "syscall.h"

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
 * returns 1 if addr was OK, 0 if illegal.
 */
int
fetchint(struct proc *p, unsigned addr, int *ip)
{
  *ip = 0;

  if(addr > p->sz - 4)
    return 0;
  memcpy(ip, p->mem + addr, 4);
  return 1;
}

int
fetcharg(int argno, int *ip)
{
  unsigned esp;

  esp = (unsigned) curproc[cpu()]->tf->tf_esp;
  return fetchint(curproc[cpu()], esp + 8 + 4*argno, ip);
}

int
putint(struct proc *p, unsigned addr, int ip)
{
  if(addr > p->sz - 4)
    return 0;
  memcpy(p->mem + addr, &ip, 4);
  return 1;
}

int
sys_pipe()
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
sys_write()
{
  int fd, n;
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
  return fd_write(p->fds[fd], p->mem + addr, n);
}

int
sys_read()
{
  int fd, n;
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
  return fd_read(p->fds[fd], p->mem + addr, n);
}

int
sys_fork()
{
  struct proc *np;

  np = newproc();
  return np->pid;
}

int
sys_exit()
{
  struct proc *p;
  struct proc *cp = curproc[cpu()];

  cp->state = ZOMBIE;

  // wake up parent
  for(p = proc; p < &proc[NPROC]; p++)
    if(p->pid == cp->ppid)
      wakeup(p);

  // abandon children
  for(p = proc; p < &proc[NPROC]; p++)
    if(p->ppid == cp->pid)
      p->pid = 1;

  swtch();

  return 0;
}

int
sys_wait()
{
  struct proc *p;
  struct proc *cp = curproc[cpu()];
  int any, pid;

  cprintf("waid pid %d ppid %d\n", cp->pid, cp->ppid);

  while(1){
    any = 0;
    for(p = proc; p < &proc[NPROC]; p++){
      if(p->state == ZOMBIE && p->ppid == cp->pid){
        kfree(p->mem, p->sz);
        kfree(p->kstack, KSTACKSIZE);
        pid = p->pid;
        p->state = UNUSED;
        cprintf("%x collected %x\n", cp, p);
        return pid;
      }
      if(p->state != UNUSED && p->ppid == cp->pid)
        any = 1;
    }
    if(any == 0){
      cprintf("%x nothing to wait for\n", cp);
      return -1;
    }
    sleep(cp);
  }
}

int
sys_cons_putc()
{
  int c;

  fetcharg(0, &c);
  cons_putc(c & 0xff);
  return 0;
}

void
syscall()
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
  default:
    cprintf("unknown sys call %d\n", num);
    // XXX fault
    break;
  }
  cp->tf->tf_regs.reg_eax = ret;
}
