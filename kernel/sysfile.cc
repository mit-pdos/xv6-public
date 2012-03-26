#include "types.h"
#include "stat.h"
#include "mmu.h"
#include "kernel.hh"
#include "spinlock.h"
#include "condvar.h"
#include "queue.h"
#include "proc.hh"
#include "fs.h"
#include "file.hh"
#include "fcntl.h"
#include "cpu.hh"
#include "net.hh"
#include "kmtrace.hh"

static bool
getfile(int fd, sref<file> *f)
{
  return myproc()->ftable->getfile(fd, f);
}

// Allocate a file descriptor for the given file.
// Takes over file reference from caller on success.
static int
fdalloc(file *f)
{
  return myproc()->ftable->allocfd(f);
}

long
sys_dup(int ofd)
{
  sref<file> f;
  int fd;
  
  if (!getfile(ofd, &f))
    return -1;
  f->inc();
  if ((fd = fdalloc(f.ptr())) < 0) {
    f->dec();
    return -1;
  }
  return fd;
}

s64
sys_read(int fd, char *p, int n)
{
  sref<file> f;

  if(!getfile(fd, &f) || argcheckptr(p, n) < 0)
    return -1;
  return f->read(p, n);
}

ssize_t
sys_pread(int fd, void *ubuf, size_t count, off_t offset)
{
  sref<file> f;
  uptr i = (uptr)ubuf;

  if (!getfile(fd, &f))
    return -1;

  for(uptr va = PGROUNDDOWN(i); va < i+count; va = va + PGSIZE)
    if(pagefault(myproc()->vmap, va, 0) < 0)
      return -1;

  return f->pread((char*)ubuf, count, offset);
}

long
sys_write(int fd, char *p, int n)
{
  sref<file> f;

  if (!getfile(fd, &f) || argcheckptr(p, n) < 0)
    return -1;
  return f->write(p, n);
}

long
sys_close(int fd)
{
  sref<file> f;
  
  if (!getfile(fd, &f))
    return -1;
  myproc()->ftable->close(fd);
  return 0;
}

long
sys_fstat(int fd, struct stat *st)
{
  sref<file> f;
  
  if (!getfile(fd, &f) || argcheckptr(st, sizeof(*st)) < 0)
    return -1;
  return f->stat(st);
}

// Create the path new as a link to the same inode as old.
long
sys_link(const char *old, const char *newn)
{
  char name[DIRSIZ];
  struct inode *dp, *ip;

  if(argcheckstr(old) < 0 || argcheckstr(newn) < 0)
    return -1;
  if((ip = namei(myproc()->cwd, old)) == 0)
    return -1;
  ilock(ip, 1);
  if(ip->type == T_DIR){
    iunlockput(ip);
    return -1;
  }
  ip->nlink++;
  iupdate(ip);
  iunlock(ip);

  if((dp = nameiparent(myproc()->cwd, newn, name)) == 0)
    goto bad;
  if(dp->dev != ip->dev || dirlink(dp, name, ip->inum) < 0)
    goto bad;

  //nc_insert(dp, name, ip);
  iput(ip);
  return 0;

bad:
  ilock(ip, 1);
  ip->nlink--;
  iupdate(ip);
  iunlockput(ip);
  return -1;
}

// Is the directory dp empty except for "." and ".." ?
static int
isdirempty(struct inode *dp)
{
  dir_init(dp);
  int empty = 1;
  dp->dir.load()->enumerate([&empty](const strbuf<DIRSIZ> &name, u64 ino)->bool{
      if (!strcmp(name._buf, "."))
        return false;
      if (!strcmp(name._buf, ".."))
        return false;
      empty = 0;
      return true;
    });
  return empty;
}

long
sys_unlink(const char *path)
{
  struct inode *ip, *dp;
  char name[DIRSIZ];

  if(argcheckstr(path) < 0)
    return -1;
  if((dp = nameiparent(myproc()->cwd, path, name)) == 0)
    return -1;
  if(dp->type != T_DIR)
    panic("sys_unlink");

  // Cannot unlink "." or "..".
  if(namecmp(name, ".") == 0 || namecmp(name, "..") == 0){
    iput(dp);
    return -1;
  }

 retry:
  if((ip = dirlookup(dp, name)) == 0){
    iput(dp);
    return -1;
  }
  ilock(ip, 1);

  if(ip->nlink < 1)
    panic("unlink: nlink < 1");
  if(ip->type == T_DIR && !isdirempty(ip)){
    iunlockput(ip);
    iput(dp);
    return -1;
  }

  dir_init(dp);
  if (dp->dir.load()->remove(strbuf<DIRSIZ>(name), &ip->inum) == 0) {
    iunlockput(ip);
    goto retry;
  }

  if(ip->type == T_DIR){
    ilock(dp, 1);
    dp->nlink--;
    iupdate(dp);
    iunlock(dp);
  }

  //nc_invalidate(dp, name);
  iput(dp);

  ip->nlink--;
  iupdate(ip);
  iunlockput(ip);
  return 0;
}

static struct inode*
create(inode *cwd, const char *path, short type, short major, short minor)
{
  struct inode *ip, *dp;
  char name[DIRSIZ];
  mt_ascope ascope("%s(%d.%d,%s,%d,%d,%d)",
                   __func__, cwd->dev, cwd->inum,
                   path, type, major, minor);

 retry:
  if((dp = nameiparent(cwd, path, name)) == 0)
    return 0;
  if(dp->type != T_DIR)
    panic("create");

  if((ip = dirlookup(dp, name)) != 0){
    iput(dp);
    ilock(ip, 1);
    if(type == T_FILE && ip->type == T_FILE)
      return ip;
    iunlockput(ip);
    return nullptr;
  }

  if((ip = ialloc(dp->dev, type)) == nullptr)
    return nullptr;

  ip->major = major;
  ip->minor = minor;
  ip->nlink = 1;
  iupdate(ip);

  mtwriteavar("inode:%x.%x", ip->dev, ip->inum);

  if(type == T_DIR){  // Create . and .. entries.
    dp->nlink++;  // for ".."
    iupdate(dp);
    // No ip->nlink++ for ".": avoid cyclic ref count.
    if(dirlink(ip, ".", ip->inum) < 0 || dirlink(ip, "..", dp->inum) < 0)
      panic("create dots");
  }

  if(dirlink(dp, name, ip->inum) < 0) {
    // create race
    ip->nlink--;
    iunlockput(ip);
    iput(dp);
    goto retry;
  }

  //nc_insert(dp, name, ip);
  iput(dp);
  return ip;
}

long
sys_openat(int dirfd, const char *path, int omode)
{
  int fd;
  struct file *f;
  struct inode *ip;
  struct inode *cwd;

  if (dirfd == AT_FDCWD) {
    cwd = myproc()->cwd;
  } else {
    // XXX(sbw) do we need the sref while we touch fdir->ip?
    sref<file> fdir;
    if (!getfile(dirfd, &fdir) || fdir->type != file::FD_INODE)
      return -1;
    cwd = fdir->ip;
  }

  if(argcheckstr(path) < 0)
    return -1;

  // Reads the dirfd FD, dirfd's inode, the inodes of all files in
  // path; writes the returned FD
  mt_ascope ascope("%s(%d,%s,%d)", __func__, dirfd, path, omode);
  mtwriteavar("thread:%x", myproc()->pid);
  mtreadavar("inode:%x.%x", cwd->dev, cwd->inum);

  if(omode & O_CREATE){
    if((ip = create(cwd, path, T_FILE, 0, 0)) == 0)
      return -1;

    // XXX necessary because the mtwriteavar() to the same abstract variable
    // does not propagate to our scope, since create() has its own inner scope.
    mtwriteavar("inode:%x.%x", ip->dev, ip->inum);
  } else {
 retry:
    if((ip = namei(cwd, path)) == 0)
      return -1;
    ilock(ip, 0);
    if(ip->type == 0) {
      iunlockput(ip);
      goto retry;
    }
    if(ip->type == T_DIR) {
      if (omode != O_RDONLY){
	iunlockput(ip);
	return -1;
      }

      dir_flush(ip);
    }
  }

  if((f = file::alloc()) == 0 || (fd = fdalloc(f)) < 0){
    if(f)
      f->dec();
    iunlockput(ip);
    return -1;
  }
  iunlock(ip);
  mtwriteavar("fd:%x.%x", myproc()->pid, fd);

  f->type = file::FD_INODE;
  f->ip = ip;
  f->off = 0;
  f->readable = !(omode & O_WRONLY);
  f->writable = (omode & O_WRONLY) || (omode & O_RDWR);
  return fd;
}

long
sys_mkdirat(int dirfd, const char *path)
{
  struct inode *cwd;
  struct inode *ip;

  if (dirfd == AT_FDCWD) {
    cwd = myproc()->cwd;
  } else {
    // XXX(sbw) do we need the sref while we touch fdir->ip?
    sref<file> fdir;
    if (!getfile(dirfd, &fdir) || fdir->type != file::FD_INODE)
      return -1;
    cwd = fdir->ip;
  }

  if (argcheckstr(path) < 0)
    return -1;
  ip = create(cwd, path, T_DIR, 0, 0);
  if (ip == nullptr)
    return -1;
  iunlockput(ip);
  return 0;
}

long
sys_mknod(const char *path, int major, int minor)
{
  struct inode *ip;
  int len;
  
  if((len=argcheckstr(path)) < 0 ||
     (ip = create(myproc()->cwd, path, T_DEV, major, minor)) == 0)
    return -1;
  iunlockput(ip);
  return 0;
}

long
sys_chdir(const char *path)
{
  struct inode *ip;

  if(argcheckstr(path) < 0 || (ip = namei(myproc()->cwd, path)) == 0)
    return -1;
  ilock(ip, 0);
  if(ip->type != T_DIR){
    iunlockput(ip);
    return -1;
  }
  iunlock(ip);
  iput(myproc()->cwd);
  myproc()->cwd = ip;
  return 0;
}

long
sys_exec(const char *path, u64 uargv)
{
  char *argv[MAXARG];
  int i;
  u64 uarg;

  if(argcheckstr(path) < 0) {
    return -1;
  }
  memset(argv, 0, sizeof(argv));
  for(i=0;; i++){
    if(i >= NELEM(argv))
      return -1;
    if(fetchint64(uargv+8*i, &uarg) < 0)
      return -1;
    if(uarg == 0){
      argv[i] = 0;
      break;
    }
    argv[i] = (char*) uarg;
    if(argcheckstr(argv[i]) < 0)
      return -1;
  }
  return exec(path, argv);
}

long
sys_pipe(int *fd)
{
  struct file *rf, *wf;
  int fd0, fd1;

  if(argcheckptr(fd, 2*sizeof(fd[0])) < 0)
    return -1;
  if(pipealloc(&rf, &wf) < 0)
    return -1;
  fd0 = -1;
  if((fd0 = fdalloc(rf)) < 0 || (fd1 = fdalloc(wf)) < 0){
    if(fd0 >= 0)
      myproc()->ftable->close(fd0);
    wf->dec();
    return -1;
  }
  fd[0] = fd0;
  fd[1] = fd1;
  return 0;
}

static void
freesocket(int fd)
{
  myproc()->ftable->close(fd);
}

static bool
getsocket(int fd, sref<file> *f)
{
  if (!getfile(fd, f))
    return false;
  if ((*f)->type != file::FD_SOCKET) {
    f->init(nullptr);
    return false;
  }
  return true;
}

static int
allocsocket(struct file **rf, int *rfd)
{
  struct file *f;
  int fd;

  f = file::alloc();
  if (f == nullptr)
    return -1;

  fd = fdalloc(f);
  if (fd < 0) {
    f->dec();
    return fd;
  }

  f->type = file::FD_SOCKET;
  f->off = 0;
  f->readable = 1;
  f->writable = 1;

  *rf = f;
  *rfd = fd;
  return 0;
}

long
sys_socket(int domain, int type, int protocol)
{
  extern long netsocket(int domain, int type, int protocol);
  struct file *f;
  int fd;
  int s;

  if (allocsocket(&f, &fd))
    return -1;

  s = netsocket(domain, type, protocol);
  if (s < 0) {
    myproc()->ftable->close(fd);
    return s;
  }

  f->socket = s;
  return fd;
}

long
sys_bind(int xsock, void *xaddr, int xaddrlen)
{
  extern long netbind(int, void*, int);
  sref<file> f;

  if (!getsocket(xsock, &f))
    return -1;

  return netbind(f->socket, xaddr, xaddrlen);
}

long
sys_listen(int xsock, int backlog)
{
  extern long netlisten(int, int);
  sref<file> f;

  if (!getsocket(xsock, &f))
    return -1;

  return netlisten(f->socket, backlog);
}

long
sys_accept(int xsock, void *xaddr, void *xaddrlen)
{
  extern long netaccept(int, void*, void*);
  file *cf;
  sref<file> f;
  int cfd;
  int ss;

  if (!getsocket(xsock, &f))
    return -1;

  if (allocsocket(&cf, &cfd))
    return -1;

  ss = netaccept(f->socket, xaddr, xaddrlen);
  if (ss < 0) {
    freesocket(cfd);
    return ss;
  }  

  cf->socket = ss;
  return cfd;
}
