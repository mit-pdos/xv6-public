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

// check that an entire string is valid in user space.
// returns the length, not including null, or -1.
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
sys_open(void)
{
  struct proc *cp = curproc[cpu()];
  struct inode *ip;
  uint arg0, arg1;
  int ufd;
  struct fd *fd;
  int l;

  if(fetcharg(0, &arg0) < 0 || fetcharg(1, &arg1) < 0)
    return -1;
  if((l = checkstring(arg0)) < 0)
    return -1;
  if((ip = namei(cp->mem + arg0, NAMEI_LOOKUP, 0)) == 0) {
    if (arg1 & O_CREATE) {
      if (l >= DIRSIZ)
	return -1;
      ip = mknod (cp->mem + arg0, T_FILE, 0, 0);
      if (ip == 0) return -1;
    } else return -1;
  }
  if((fd = fd_alloc()) == 0){
    iput(ip);
    return -1;
  }
  if((ufd = fd_ualloc()) < 0){
    iput(ip);
    fd_close(fd);
    return -1;
  }

  iunlock(ip);
  fd->type = FD_FILE;
  if (arg1 & O_RDWR) {
    fd->readable = 1;
    fd->writeable = 1;
  } else if (arg1 & O_WRONLY) {
    fd->readable = 0;
    fd->writeable = 1;
  } else {
    fd->readable = 1;
    fd->writeable = 0;
  }
  fd->ip = ip;
  fd->off = 0;
  cp->fds[ufd] = fd;

  return ufd;
}

int
sys_mknod(void)
{
  struct proc *cp = curproc[cpu()];
  struct inode *nip;
  uint arg0, arg1, arg2, arg3;
  int l;

  if(fetcharg(0, &arg0) < 0 || fetcharg(1, &arg1) < 0 || 
     fetcharg(2, &arg2) < 0 || fetcharg(3, &arg3) < 0)
    return -1;

  if((l = checkstring(arg0)) < 0)
    return -1;

  if(l >= DIRSIZ)
    return -1;

  nip = mknod (cp->mem + arg0, (short) arg1, (short) arg2, (short) arg3);
  if(nip)
    iput(nip);
  return (nip == 0) ? -1 : 0;
}

int
sys_mkdir(void)
{
  struct proc *cp = curproc[cpu()];
  struct inode *nip;
  uint arg0;
  int l;

  if(fetcharg(0, &arg0) < 0) 
    return -1;

  if((l = checkstring(arg0)) < 0)
    return -1;

  if(l >= DIRSIZ)
    return -1;

  nip = mknod (cp->mem + arg0, T_DIR, 0, 0);

  // XXX put . and .. in

  iput(nip);
  return (nip == 0) ? -1 : 0;
}

int
sys_unlink(void)
{
  struct proc *cp = curproc[cpu()];
  uint arg0;
  int r;
  
  if(fetcharg(0, &arg0) < 0)
    return -1;
  if(checkstring(arg0) < 0)
    return -1;
  r = unlink(cp->mem + arg0);
  return r;
}

int
sys_fstat(void)
{
  struct proc *cp = curproc[cpu()];
  uint fd, addr;
  int r;
  
  if(fetcharg(0, &fd) < 0)
    return -1;
  if(fetcharg(1, &addr) < 0)
    return -1;
  if(fd < 0 || fd >= NOFILE)
    return -1;
  if(cp->fds[fd] == 0)
    return -1;
  if(addr + sizeof(struct stat) > cp->sz)
    return -1;
  r = fd_stat (cp->fds[fd], (struct stat *)(cp->mem + addr));
  return r;
}

int
sys_link(void)
{
  struct proc *cp = curproc[cpu()];
  uint name1, name2;
  int r;
  
  if(fetcharg(0, &name1) < 0 || checkstring(name1) < 0)
    return -1;
  if(fetcharg(1, &name2) < 0 || checkstring(name2) < 0)
    return -1;
  r = link(cp->mem + name1, cp->mem + name2);
  return r;
}

int
sys_exec(void)
{
  struct proc *cp = curproc[cpu()];
  uint arg0, arg1, sz=0, ap, sp, p1, p2;
  int i, nargs, argbytes, len;
  struct inode *ip;
  struct elfhdr elf;
  struct proghdr ph;
  char *mem = 0;

  if(fetcharg(0, &arg0) < 0)
    return -1;
  if(fetcharg(1, &arg1) < 0)
    return -1;
  if(checkstring(arg0) < 0)
    return -1;
  ip = namei(cp->mem + arg0, NAMEI_LOOKUP, 0);
  if(ip == 0)
    return -1;

  if(readi(ip, (char*)&elf, 0, sizeof(elf)) < sizeof(elf))
    goto bad;

  if(elf.magic != ELF_MAGIC)
    goto bad;

  sz = 0;
  for(i = 0; i < elf.phnum; i++){
    if(readi(ip, (char*)&ph, elf.phoff + i * sizeof(ph),
             sizeof(ph)) != sizeof(ph))
      goto bad;
    if(ph.type != ELF_PROG_LOAD)
      continue;
    if(ph.memsz < ph.filesz)
      goto bad;
    sz += ph.memsz;
  }
  
  sz += 4096 - (sz % 4096);
  sz += 4096;

  mem = kalloc(sz);
  if(mem == 0)
    goto bad;
  memset(mem, 0, sz);

  // arg1 is a pointer to an array of pointers to string.
  nargs = 0;
  argbytes = 0;
  for(i = 0; ; i++){
    if(fetchint(cp, arg1 + 4*i, &ap) < 0)
      goto bad;
    if(ap == 0)
      break;
    len = checkstring(ap);
    if(len < 0)
      goto bad;
    nargs++;
    argbytes += len + 1;
  }

  // argn\0
  // ...
  // arg0\0
  // 0
  // ptr to argn
  // ...
  // 12: ptr to arg0
  //  8: argv (points to ptr to arg0)
  //  4: argc
  //  0: fake return pc
  sp = sz - argbytes - (nargs+1)*4 - 4 - 4 - 4;
  *(uint*)(mem + sp) = 0xffffffff;
  *(uint*)(mem + sp + 4) = nargs;
  *(uint*)(mem + sp + 8) = (uint)(sp + 12);

  p1 = sp + 12;
  p2 = sp + 12 + (nargs + 1) * 4;
  for(i = 0; i < nargs; i++){
    fetchint(cp, arg1 + 4*i, &ap);
    len = checkstring(ap);
    memmove(mem + p2, cp->mem + ap, len + 1);
    *(uint*)(mem + p1) = p2;
    p1 += 4;
    p2 += len + 1;
  }
  *(uint*)(mem + p1) = 0;

  // commit to the new image.
  kfree(cp->mem, cp->sz);
  cp->sz = sz;
  cp->mem = mem;
  mem = 0;

  for(i = 0; i < elf.phnum; i++){
    if(readi(ip, (char*)&ph, elf.phoff + i * sizeof(ph),
             sizeof(ph)) != sizeof(ph))
      goto bad2;
    if(ph.type != ELF_PROG_LOAD)
      continue;
    if(ph.va + ph.memsz > sz)
      goto bad2;
    if(readi(ip, cp->mem + ph.va, ph.offset, ph.filesz) != ph.filesz)
      goto bad2;
    memset(cp->mem + ph.va + ph.filesz, 0, ph.memsz - ph.filesz);
  }

  iput(ip);

  cp->tf->eip = elf.entry;
  cp->tf->esp = sp;
  setupsegs(cp);

  return 0;

 bad:
  cprintf("exec failed early\n");
  if(mem)
    kfree(mem, sz);
  iput(ip);
  return -1;

 bad2:
  cprintf("exec failed late\n");
  iput(ip);
  proc_exit();
  return 0;
}

int
sys_block(void)
{
  int i, j;
  struct buf *b;
  struct inode *ip;

  for (i = 0; i < 2; i++) {
    cprintf ("issue read\n");
    b = bread(1, i);

    cprintf("disk 1 sector %d: ", i);
    for (j = 0; j < 4; j++)
      cprintf("%x ", b->data[j] & 0xff);
    cprintf("\n");

    brelse(b);
  }

  ip = iget(1, 1);
  cprintf("iget 1: %d %d %d %d %d %d %d %d\n",
          ip->dev, ip->inum, ip->count, ip->busy,
          ip->type, ip->nlink, ip->size, ip->addrs[0]);
  iput(ip);

  ip = namei(".././//./../usertests", NAMEI_LOOKUP, 0);
  if(ip){
    cprintf("namei(usertests): %d %d %d %d %d %d %d %d\n",
            ip->dev, ip->inum, ip->count, ip->busy,
            ip->type, ip->nlink, ip->size, ip->addrs[0]);
    iput(ip);
  } else {
    cprintf("namei(usertests) failed\n");
  }

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
  default:
    cprintf("unknown sys call %d\n", num);
    // XXX fault
    break;
  }
  cp->tf->eax = ret;
}
