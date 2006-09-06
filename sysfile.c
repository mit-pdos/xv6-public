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

int
sys_pipe(void)
{
  struct file *rfd = 0, *wfd = 0;
  int f1 = -1, f2 = -1;
  struct proc *p = curproc[cpu()];
  uint fdp;

  if(pipe_alloc(&rfd, &wfd) < 0)
    goto oops;
  if((f1 = fdalloc()) < 0)
    goto oops;
  p->ofile[f1] = rfd;
  if((f2 = fdalloc()) < 0)
    goto oops;
  p->ofile[f2] = wfd;
  if(fetcharg(0, &fdp) < 0)
    goto oops;
  if(putint(p, fdp, f1) < 0)
    goto oops;
  if(putint(p, fdp+4, f2) < 0)
    goto oops;
  return 0;

 oops:
  if(rfd)
    fileclose(rfd);
  if(wfd)
    fileclose(wfd);
  if(f1 >= 0)
    p->ofile[f1] = 0;
  if(f2 >= 0)
    p->ofile[f2] = 0;
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
  if(p->ofile[fd] == 0)
    return -1;
  if(addr + n > p->sz)
    return -1;

  ret = filewrite(p->ofile[fd], p->mem + addr, n);
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
  if(p->ofile[fd] == 0)
    return -1;
  if(addr + n > p->sz)
    return -1;
  ret = fileread(p->ofile[fd], p->mem + addr, n);
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
  if(p->ofile[fd] == 0)
    return -1;
  fileclose(p->ofile[fd]);
  p->ofile[fd] = 0;
  return 0;
}

int
sys_open(void)
{
  struct proc *cp = curproc[cpu()];
  struct inode *ip, *dp;
  uint arg0, arg1;
  int ufd;
  struct file *fd;
  int l;
  char *last;

  if(fetcharg(0, &arg0) < 0 || fetcharg(1, &arg1) < 0)
    return -1;
  if((l = checkstring(arg0)) < 0)
    return -1;

  if(arg1 & O_CREATE){
    dp = namei(cp->mem + arg0, NAMEI_CREATE, 0, &last, &ip);
    if(dp){
      ip = mknod1(dp, last, T_FILE, 0, 0);
      iput(dp);
      if(ip == 0)
        return -1;
    } else if(ip == 0){
      return -1;
    } else if(ip->type == T_DIR){
      iput(ip);
      return -1;
    }
  } else {
    ip = namei(cp->mem + arg0, NAMEI_LOOKUP, 0, 0, 0);
    if(ip == 0)
      return -1;
  }
  if(ip->type == T_DIR && ((arg1 & O_RDWR) || (arg1 & O_WRONLY))){
    iput(ip);
    return -1;
  }

  if((fd = filealloc()) == 0){
    iput(ip);
    return -1;
  }
  if((ufd = fdalloc()) < 0){
    iput(ip);
    fileclose(fd);
    return -1;
  }

  iunlock(ip);
  fd->type = FD_FILE;
  if(arg1 & O_RDWR) {
    fd->readable = 1;
    fd->writable = 1;
  } else if(arg1 & O_WRONLY) {
    fd->readable = 0;
    fd->writable = 1;
  } else {
    fd->readable = 1;
    fd->writable = 0;
  }
  fd->ip = ip;
  fd->off = 0;
  cp->ofile[ufd] = fd;

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

  nip = mknod(cp->mem + arg0, (short) arg1, (short) arg2, (short) arg3);
  if(nip)
    iput(nip);
  return (nip == 0) ? -1 : 0;
}

int
sys_mkdir(void)
{
  struct proc *cp = curproc[cpu()];
  struct inode *nip;
  struct inode *dp;
  uint arg0;
  int l;
  struct dirent de;
  char *last;

  if(fetcharg(0, &arg0) < 0)
    return -1;

  if((l = checkstring(arg0)) < 0)
    return -1;

  dp = namei(cp->mem + arg0, NAMEI_CREATE, 0, &last, 0);
  if(dp == 0)
    return -1;

  nip = mknod1(dp, last, T_DIR, 0, 0);
  if(nip == 0){
    iput(dp);
    return -1;
  }

  dp->nlink += 1;
  iupdate(dp);

  memset(de.name, '\0', DIRSIZ);
  de.name[0] = '.';
  de.inum = nip->inum;
  writei(nip, (char*) &de, 0, sizeof(de));

  de.inum = dp->inum;
  de.name[1] = '.';
  writei(nip, (char*) &de, sizeof(de), sizeof(de));

  iput(dp);
  iput(nip);

  return 0;
}


int
sys_chdir(void)
{
  struct proc *cp = curproc[cpu()];
  struct inode *ip;
    uint arg0;
  int l;

  if(fetcharg(0, &arg0) < 0)
    return -1;

  if((l = checkstring(arg0)) < 0)
    return -1;

  if((ip = namei(cp->mem + arg0, NAMEI_LOOKUP, 0, 0, 0)) == 0)
    return -1;

  if(ip == cp->cwd) {
    iput(ip);
    return 0;
  }

  if(ip->type != T_DIR) {
    iput(ip);
    return -1;
  }

  idecref(cp->cwd);
  cp->cwd = ip;
  iunlock(cp->cwd);
  return 0;
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
  if(cp->ofile[fd] == 0)
    return -1;
  if(addr + sizeof(struct stat) > cp->sz)
    return -1;
  r = filestat(cp->ofile[fd], (struct stat*)(cp->mem + addr));
  return r;
}

int
sys_dup(void)
{
  struct proc *cp = curproc[cpu()];
  uint fd, ufd1;

  if(fetcharg(0, &fd) < 0)
    return -1;
  if(fd < 0 || fd >= NOFILE)
    return -1;
  if(cp->ofile[fd] == 0)
    return -1;
  if((ufd1 = fdalloc()) < 0)
    return -1;
  cp->ofile[ufd1] = cp->ofile[fd];
  fileincref(cp->ofile[ufd1]);
  return ufd1;
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
  ip = namei(cp->mem + arg0, NAMEI_LOOKUP, 0, 0, 0);
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
  if(mem)
    kfree(mem, sz);
  iput(ip);
  return -1;

 bad2:
  iput(ip);
  proc_exit();
  return 0;
}
