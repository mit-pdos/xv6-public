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
sys_fstat(void)
{
  struct file *f;
  struct stat *st;
  
  if(argfd(0, 0, &f) < 0 || argptr(1, (void*)&st, sizeof(*st)) < 0)
    return -1;
  return filestat(f, st);
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

// Create the path new as a link to the same inode as old.
int
sys_link(void)
{
  char name[DIRSIZ], *new, *old;
  struct inode *dp, *ip;
  struct uinode *ipu;

  if(argstr(0, &old) < 0 || argstr(1, &new) < 0)
    return -1;
  if((ip = ilock(namei(old))) == 0)
    return -1;
  if(ip->type == T_DIR){
    iput(iunlock(ip));
    return -1;
  }
  ip->nlink++;
  iupdate(ip);
  ipu = iunlock(ip);  ip = 0;

  if((dp = ilock(nameiparent(new, name))) == 0 ||
     dp->dev != ipu->dev || dirlink(dp, name, ipu->inum) < 0){
    if(dp)
      iput(iunlock(dp));
    ip = ilock(ipu);
    ip->nlink--;
    iupdate(ip);
    iput(iunlock(ip));
    return -1;
  }
  iput(iunlock(dp));
  iput(ipu);
  return 0;
}

// Is the directory dp empty except for "." and ".." ?
static int
isdirempty(struct inode *dp)
{
  int off;
  struct dirent de;

  for(off=2*sizeof(de); off<dp->size; off+=sizeof(de)){
    if(readi(dp, (char*)&de, off, sizeof(de)) != sizeof(de))
      panic("isdirempty: readi");
    if(de.inum != 0)
      return 0;
  }
  return 1;
}

int
sys_unlink(void)
{
  struct inode *ip, *dp;
  struct dirent de;
  char name[DIRSIZ], *path;
  uint off;

  if(argstr(0, &path) < 0)
    return -1;
  if((dp = ilock(nameiparent(path, name))) == 0)
    return -1;

  // Cannot unlink "." or "..".
  if(namecmp(name, ".") == 0 || namecmp(name, "..") == 0){
    iput(iunlock(dp));
    return -1;
  }

  if((ip = ilock(dirlookup(dp, name, &off))) == 0){
    iput(iunlock(dp));
    return -1;
  }

  if(ip->nlink < 1)
    panic("unlink: nlink < 1");
  if(ip->type == T_DIR && !isdirempty(ip)){
    iput(iunlock(ip));
    iput(iunlock(dp));
    return -1;
  }

  memset(&de, 0, sizeof(de));
  if(writei(dp, (char*)&de, off, sizeof(de)) != sizeof(de))
    panic("unlink: writei");
  iput(iunlock(dp));
  
  ip->nlink--;
  iupdate(ip);
  iput(iunlock(ip));
  return 0;
}

// Create the path and return its unlocked inode structure.
static struct inode*
mkpath(char *path, int canexist, short type, short major, short minor)
{
  uint off;
  struct inode *ip, *dp;
  struct uinode *ipu;
  char name[DIRSIZ];

  if((dp = ilock(nameiparent(path, name))) == 0)
    return 0;

  if(canexist && (ipu = dirlookup(dp, name, &off)) != 0){
    iput(iunlock(dp));
    ip = ilock(ipu);
    if(ip->type != type || ip->major != major || ip->minor != minor){
      iput(iunlock(ip));
      return 0;
    }
    return ip;
  }

  if((ip = ilock(ialloc(dp->dev, type))) == 0){
    iput(iunlock(dp));
    return 0;
  }
  ip->major = major;
  ip->minor = minor;
  ip->size = 0;
  ip->nlink = 1;
  iupdate(ip);
  
  if(dirlink(dp, name, ip->inum) < 0){
    ip->nlink = 0;
    iput(iunlock(ip));
    iput(iunlock(dp));
    return 0;
  }

  if(type == T_DIR){  // Create . and .. entries.
    dp->nlink++;  // for ".."
    iupdate(dp);
    // No ip->nlink++ for ".": avoid cyclic ref count.
    if(dirlink(ip, ".", ip->inum) < 0 || dirlink(ip, "..", dp->inum) < 0)
      panic("mkpath dots");
  }
  iput(iunlock(dp));
  return ip;
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

  if(omode & O_CREATE){
    if((ip = mkpath(path, 1, T_FILE, 0, 0)) == 0)
      return -1;
  }else{
    if((ip = ilock(namei(path))) == 0)
      return -1;
    if(ip->type == T_DIR && (omode & (O_RDWR|O_WRONLY))){
      iput(iunlock(ip));
      return -1;
    }
  }

  if((f = filealloc()) == 0 || (fd = fdalloc(f)) < 0){
    if(f)
      fileclose(f);
    iput(iunlock(ip));
    return -1;
  }

  f->type = FD_INODE;
  f->ip = iunlock(ip);
  f->off = 0;
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

  return fd;
}

int
sys_mknod(void)
{
  struct inode *ip;
  char *path;
  int len;
  int type, major, minor;
  
  if((len=argstr(0, &path)) < 0 || argint(1, &type) < 0 ||
     argint(2, &major) < 0 || argint(3, &minor) < 0)
    return -1;
  // XXX check that type == T_DEV or eliminate type arg?
  if((ip = mkpath(path, 0, type, major, minor)) == 0)
    return -1;
  iput(iunlock(ip));
  return 0;
}

int
sys_mkdir(void)
{
  char *path;
  struct inode *ip;

  if(argstr(0, &path) < 0 || (ip = mkpath(path, 0, T_DIR, 0, 0)) == 0)
    return -1;
  iput(iunlock(ip));
  return 0;
}

int
sys_chdir(void)
{
  char *path;
  struct inode *ip;

  if(argstr(0, &path) < 0 || (ip = ilock(namei(path))) == 0)
    return -1;
  if(ip->type != T_DIR) {
    iput(iunlock(ip));
    return -1;
  }
  iput(cp->cwd);
  cp->cwd = iunlock(ip);
  return 0;
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
sys_exec(void)
{
  char *path, *argv[20];
  int i;
  uint uargv, uarg;

  if(argstr(0, &path) < 0 || argint(1, (int*)&uargv) < 0)
    return -1;
  memset(argv, 0, sizeof(argv));
  for(i=0;; i++){
    if(i >= NELEM(argv))
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

int
sys_pipe(void)
{
  int *fd;
  struct file *rf, *wf;
  int fd0, fd1;

  if(argptr(0, (void*)&fd, 2*sizeof(fd[0])) < 0)
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
