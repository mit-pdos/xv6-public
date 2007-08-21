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

  if(argint(argno, &fd) < 0)
    return -1;
  if(fd < 0 || fd >= NOFILE || (f=cp->ofile[fd]) == 0)
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

  for(fd = 0; fd < NOFILE; fd++){
    if(cp->ofile[fd] == 0){
      cp->ofile[fd] = f;
      return fd;
    }
  }
  return -1;
}

int
sys_pipe(void)
{
  int *fd;
  struct file *rf, *wf;
  int fd0, fd1;

  if(argptr(0, (void*)&fd, 2*sizeof fd[0]) < 0)
    return -1;
  if(pipe_alloc(&rf, &wf) < 0)
    return -1;
  fd0 = -1;
  if((fd0 = fdalloc(rf)) < 0 || (fd1 = fdalloc(wf)) < 0){
    if(fd0 >= 0)
      cp->ofile[fd0] = 0;
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
  cp->ofile[fd] = 0;
  fileclose(f);
  return 0;
}

int
sys_open(void)
{
  char *path;
  int fd, omode;
  struct file *f;
  struct inode *ip;

  if(argstr(0, &path) < 0 || argint(1, &omode) < 0)
    return -1;

  if(omode & O_CREATE)
    ip = create(path);
  else
    ip = namei(path);
  if(ip == 0)
    return -1;

  if(ip->type == T_DIR && (omode & (O_RDWR|O_WRONLY))){
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

  // XXX why this check?
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
  char *path;

  if(argstr(0, &path) < 0)
    return -1;
  return mkdir(path);
}

int
sys_chdir(void)
{
  struct inode *ip;
  char *path;

  if(argstr(0, &path) < 0)
    return -1;

  if((ip = namei(path)) == 0)
    return -1;

  if(ip->type != T_DIR) {
    iput(ip);
    return -1;
  }

  iunlock(ip);
  idecref(cp->cwd);
  cp->cwd = ip;
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

#define ARGMAX 10

int
sys_exec(void)
{
  char *path, *argv[ARGMAX];
  int i;
  uint uargv, uarg;

  if(argstr(0, &path) < 0 || argint(1, (int*)&uargv) < 0)
    return -1;
  memset(argv, 0, sizeof argv);
  for(i=0;; i++){
    if(i >= ARGMAX)
      return -1;
    if(fetchint(cp, uargv+4*i, (int*)&uarg) < 0)
      return -1;
    if(uarg == 0){
      argv[i] = 0;
      break;
    }
    if(fetchstr(cp, uarg, &argv[i]) < 0)
      return -1;
  }
  return exec(path, argv);
}

