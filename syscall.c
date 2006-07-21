#include "types.h"
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

/*
 * User code makes a system call with INT T_SYSCALL.
 * System call number in %eax.
 * Arguments on the stack, from the user call to the C
 * library system call function. The saved user %esp points
 * to a saved program counter, and then the first argument.
 *
 * Return value? Error indication? Errno?
 */

/*
 * fetch 32 bits from a user-supplied pointer.
 * returns 0 if addr was OK, -1 if illegal.
 */
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
fetchbyte(struct proc *p, uint addr, char* c)
{
  if(addr >= p->sz)
    return -1;
  *c = *(p->mem + addr);
  return 0;
}

// This arg is void* so that both int* and uint* can be passed.
int
fetcharg(int argno, void *ip)
{
  uint esp;

  esp = (uint) curproc[cpu()]->tf->esp;
  return fetchint(curproc[cpu()], esp + 4 + 4*argno, ip);
}

int
putint(struct proc *p, uint addr, int x)
{
  if(addr > p->sz - 4)
    return -1;
  memmove(p->mem + addr, &x, 4);
  return 0;
}

int
sys_pipe(void)
{
  struct fd *rfd = 0, *wfd = 0;
  int f1 = -1, f2 = -1;
  struct proc *p = curproc[cpu()];
  uint fdp;

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
  uint addr;
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
  uint addr;
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

  if((np = copyproc(curproc[cpu()])) == 0)
    return -1;
  np->state = RUNNABLE;
  return np->pid;
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

  if(fetcharg(0, &pid) < 0)
    return -1;
  return proc_kill(pid);
}

int
sys_cons_putc(void)
{
  int c;
  char buf[2];

  if(fetcharg(0, &c) < 0)
    return -1;
  buf[0] = c;
  buf[1] = 0;
  cprintf("%s", buf);
  return 0;
}

int
sys_cons_puts(void)
{
  char buf[256];
  int i;
  uint addr;
  struct proc *cp = curproc[cpu()];

  if(fetcharg(0, &addr) < 0)
    return -1;
  for(i=0; i<sizeof buf-1 && fetchbyte(cp, addr+i, &buf[i]) >= 0; i++)
    if(buf[i] == 0)
      break;
  buf[i] = 0;
  cprintf("%s", buf);
  return 0;
}

int
sys_block(void)
{
  int i, j;
  struct buf *b;
  struct inode *ip;

  for (i = 0; i < 2; i++) {
    b = bread(1, i);

    cprintf("disk 1 sector %d: ", i);
    for (j = 0; j < 4; j++)
      cprintf("%x ", b->data[j] & 0xff);
    cprintf("\n");

    brelse(b);
  }

  ip = iget(1, 1);
  cprintf("%d %d %d %d %d %d %d %d\n",
          ip->dev, ip->inum, ip->count, ip->busy,
          ip->type, ip->nlink, ip->size, ip->addrs[0]);
  iput(ip);

  return 0;
}

int
sys_panic(void)
{
  struct proc *p = curproc[cpu()];
  uint addr;

  if(fetcharg(0, &addr) < 0)
    return -1;
  panic(p->mem + addr);
  return 0;
}

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
  case SYS_cons_puts:
    ret = sys_cons_puts();
    break;
  default:
    cprintf("unknown sys call %d\n", num);
    // XXX fault
    break;
  }
  cp->tf->eax = ret;
}
