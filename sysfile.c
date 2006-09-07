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

// Fetch the nth word-sized system call argument as a file descriptor
// and return both the descriptor and the corresponding struct file.
static int
argfd(int argno, int *pfd, struct file **pf)
{
  int fd;
  struct file *f;
  struct proc *p = curproc[cpu()];

  if(argint(argno, &fd) < 0)
    return -1;
  if(fd < 0 || fd >= NOFILE || (f=p->ofile[fd]) == 0)
    return -1;
  if(pfd)
    *pfd = fd;
  if(pf)
    *pf = f;
  return 0;
}

// Allocate a file descriptor for the given file.
// Takes over file reference from caller on success.
static int
fdalloc(struct file *f)
{
  int fd;
  struct proc *p = curproc[cpu()];
  for(fd = 0; fd < NOFILE; fd++){
    if(p->ofile[fd] == 0){
      p->ofile[fd] = f;
      return fd;
    }
  }
  return -1;
}

int
sys_pipe(void)
{
  int *fd;
  struct file *rf = 0, *wf = 0;
  int fd0, fd1;
  struct proc *p = curproc[cpu()];

  if(argptr(0, (void*)&fd, 2*sizeof fd[0]) < 0)
    return -1;
  if(pipe_alloc(&rf, &wf) < 0)
    return -1;
  fd0 = -1;
  if((fd0 = fdalloc(rf)) < 0 || (fd1 = fdalloc(wf)) < 0){
    if(fd0 >= 0)
      p->ofile[fd0] = 0;
    fileclose(rf);
    fileclose(wf);
    return -1;
  }
  fd[0] = fd0;
  fd[1] = fd1;
  return 0;
}

int
sys_write(void)
{
  struct file *f;
  int n;
  char *cp;

  if(argfd(0, 0, &f) < 0 || argint(2, &n) < 0 || argptr(1, &cp, n) < 0)
    return -1;
  return filewrite(f, cp, n);
}

int
sys_read(void)
{
  struct file *f;
  int n;
  char *cp;

  if(argfd(0, 0, &f) < 0 || argint(2, &n) < 0 || argptr(1, &cp, n) < 0)
    return -1;
  return fileread(f, cp, n);
}

int
sys_close(void)
{
  int fd;
  struct file *f;
  
  if(argfd(0, &fd, &f) < 0)
    return -1;
  curproc[cpu()]->ofile[fd] = 0;
  fileclose(f);
  return 0;
}

int
sys_open(void)
{
  struct inode *ip, *dp;
  char *path;
  int omode;
  int fd;
  struct file *f;
  char *last;

  if(argstr(0, &path) < 0 || argint(1, &omode) < 0)
    return -1;

  if(omode & O_CREATE){
    dp = namei(path, NAMEI_CREATE, 0, &last, &ip);
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
    ip = namei(path, NAMEI_LOOKUP, 0, 0, 0);
    if(ip == 0)
      return -1;
  }
  if(ip->type == T_DIR && ((omode & O_RDWR) || (omode & O_WRONLY))){
    iput(ip);
    return -1;
  }

  if((f = filealloc()) == 0){
    iput(ip);
    return -1;
  }
  if((fd = fdalloc(f)) < 0){
    iput(ip);
    fileclose(f);
    return -1;
  }

  iunlock(ip);
  f->type = FD_FILE;
  if(omode & O_RDWR) {
    f->readable = 1;
    f->writable = 1;
  } else if(omode & O_WRONLY) {
    f->readable = 0;
    f->writable = 1;
  } else {
    f->readable = 1;
    f->writable = 0;
  }
  f->ip = ip;
  f->off = 0;

  return fd;
}

int
sys_mknod(void)
{
  struct inode *nip;
  char *path;
  int len;
  int type, major, minor;
  
  if((len=argstr(0, &path)) < 0 || argint(1, &type) < 0 ||
     argint(2, &major) < 0 || argint(3, &minor) < 0)
    return -1;

  if(len >= DIRSIZ)
    return -1;

  if((nip = mknod(path, type, major, minor)) == 0)
    return -1;
  iput(nip);
  return 0;
}

int
sys_mkdir(void)
{
  struct inode *nip;
  struct inode *dp;
  char *path;
  struct dirent de;
  char *last;

  if(argstr(0, &path) < 0)
    return -1;

  dp = namei(path, NAMEI_CREATE, 0, &last, 0);
  if(dp == 0)
    return -1;

  nip = mknod1(dp, last, T_DIR, 0, 0);
  if(nip == 0){
    iput(dp);
    return -1;
  }

  dp->nlink++;
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
  struct proc *p = curproc[cpu()];
  struct inode *ip;
  char *path;

  if(argstr(0, &path) < 0)
    return -1;

  if((ip = namei(path, NAMEI_LOOKUP, 0, 0, 0)) == 0)
    return -1;

  if(ip == p->cwd) {
    iput(ip);
    return 0;
  }

  if(ip->type != T_DIR) {
    iput(ip);
    return -1;
  }

  idecref(p->cwd);
  p->cwd = ip;
  iunlock(p->cwd);
  return 0;
}

int
sys_unlink(void)
{
  char *path;
  
  if(argstr(0, &path) < 0)
    return -1;
  return unlink(path);
}

int
sys_fstat(void)
{
  struct file *f;
  struct stat *st;
  
  if(argfd(0, 0, &f) < 0 || argptr(1, (void*)&st, sizeof *st) < 0)
    return -1;
  return filestat(f, st);
}

int
sys_dup(void)
{
  struct file *f;
  int fd;
  
  if(argfd(0, 0, &f) < 0)
    return -1;
  if((fd=fdalloc(f)) < 0)
    return -1;
  fileincref(f);
  return fd;
}

int
sys_link(void)
{
  char *old, *new;
  
  if(argstr(0, &old) < 0 || argstr(1, &new) < 0)
    return -1;
  return link(old, new);
}

int
sys_exec(void)
{
  struct proc *cp = curproc[cpu()];
  uint sz=0, ap, sp, p1, p2;
  int i, nargs, argbytes, len;
  struct inode *ip;
  struct elfhdr elf;
  struct proghdr ph;
  char *mem = 0;
  char *path, *s;
  uint argv;
  
  if(argstr(0, &path) < 0 || argint(1, (int*)&argv) < 0)
    return -1;

  ip = namei(path, NAMEI_LOOKUP, 0, 0, 0);
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

  nargs = 0;
  argbytes = 0;
  for(i = 0;; i++){
    if(fetchint(cp, argv + 4*i, (int*)&ap) < 0)
      goto bad;
    if(ap == 0)
      break;
    len = fetchstr(cp, ap, &s);
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
    fetchint(cp, argv + 4*i, (int*)&ap);
    len = fetchstr(cp, ap, &s);
    memmove(mem + p2, s, len + 1);
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
