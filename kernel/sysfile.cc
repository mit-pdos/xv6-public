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
#include "sperf.hh"
#include "dirns.hh"

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

//SYSCALL
int
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

//SYSCALL
ssize_t
sys_read(int fd, void *p, size_t n)
{
  sref<file> f;

  if(!getfile(fd, &f) || argcheckptr(p, n) < 0)
    return -1;
  return f->read(static_cast<char*>(p), n);
}

//SYSCALL
ssize_t
sys_pread(int fd, void *ubuf, size_t count, off_t offset)
{
  sref<file> f;
  if (!getfile(fd, &f))
    return -1;

  if (count < PGSIZE) {
    ssize_t r;
    char* b;
    b = kalloc("preadbuf");
    r = f->pread(b, count, offset);
    if (r > 0)
      putmem(ubuf, b, r);
    kfree(b);
    return r;
  } else {
    // XXX(sbw) pagefaulting doesn't guarantee ubuf is mapped 
    // while pread executes
    uptr i = (uptr)ubuf;
    for(uptr va = PGROUNDDOWN(i); va < i+count; va = va + PGSIZE)
      if(pagefault(myproc()->vmap, va, 0) < 0)
        return -1;
    return f->pread((char*)ubuf, count, offset);
  }
}

//SYSCALL
ssize_t
sys_write(int fd, const void *p, size_t n)
{
  sref<file> f;

  if (!getfile(fd, &f) || argcheckptr(p, n) < 0)
    return -1;
  return f->write(static_cast<const char*>(p), n);
}

//SYSCALL
ssize_t
sys_pwrite(int fd, const void *ubuf, size_t count, off_t offset)
{
  sref<file> f;
  if (!getfile(fd, &f))
    return -1;

  if (count < PGSIZE) {
    ssize_t r;
    char* b;
    b = kalloc("pwritebuf");
    fetchmem(b, ubuf, count);
    r = f->pwrite(b, count, offset);
    kfree(b);
    return r;
  } else {
    // XXX(sbw) pagefaulting doesn't guarantee ubuf is mapped 
    // while pread executes
    uptr i = (uptr)ubuf;
    for(uptr va = PGROUNDDOWN(i); va < i+count; va = va + PGSIZE)
      if(pagefault(myproc()->vmap, va, 0) < 0)
        return -1;
    return f->pwrite((char*)ubuf, count, offset);
  }
}

//SYSCALL
int
sys_close(int fd)
{
  sref<file> f;
  
  if (!getfile(fd, &f))
    return -1;
  myproc()->ftable->close(fd);
  return 0;
}

//SYSCALL
int
sys_fstat(int fd, struct stat *st)
{
  sref<file> f;
  
  if (!getfile(fd, &f) || argcheckptr(st, sizeof(*st)) < 0)
    return -1;
  return f->stat(st);
}

// Create the path new as a link to the same inode as old.
//SYSCALL
int
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
  ip->link();
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
  ip->unlink();
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

//SYSCALL
int
sys_unlink(const char *path)
{
  struct inode *ip, *dp;
  char name[DIRSIZ];
  bool haveref = false;

  if(argcheckstr(path) < 0)
    return -1;

  scoped_gc_epoch e;
  if((dp = __nameiparent(myproc()->cwd, path, name, &haveref)) == 0)
    return -1;
  if(dp->type != T_DIR)
    panic("sys_unlink");

  // Cannot unlink "." or "..".
  if(namecmp(name, ".") == 0 || namecmp(name, "..") == 0){
    iput(dp, haveref);
    return -1;
  }

 retry:
  if((ip = dirlookup(dp, name)) == 0){
    iput(dp, haveref);
    return -1;
  }
  ilock(ip, 1);

  if(ip->nlink() < 1)
    panic("unlink: nlink < 1");
  if(ip->type == T_DIR && !isdirempty(ip)){
    iunlockput(ip);
    iput(dp, haveref);
    return -1;
  }

  dir_init(dp);
  if (dp->dir.load()->remove(strbuf<DIRSIZ>(name), &ip->inum) == 0) {
    iunlockput(ip);
    goto retry;
  }

  if(ip->type == T_DIR){
    ilock(dp, 1);
    dp->unlink();
    iupdate(dp);
    iunlock(dp);
  }

  iput(dp, haveref);

  ip->unlink();
  iupdate(ip);
  iunlockput(ip);
  return 0;
}

static struct inode*
create(inode *cwd, const char *path, short type, short major, short minor)
{
  struct inode *ip, *dp;
  char name[DIRSIZ];
  bool haveref = false;

  mt_ascope ascope("%s(%d.%d,%s,%d,%d,%d)",
                   __func__, cwd->dev, cwd->inum,
                   path, type, major, minor);

 retry:
  {
    scoped_gc_epoch e;
    if((dp = __nameiparent(cwd, path, name, &haveref)) == 0)
      return 0;

    if(dp->type != T_DIR)
      panic("create");

    if((ip = dirlookup(dp, name)) != 0){
      iput(dp, haveref);
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
    ip->link();
    iupdate(ip);
    
    mtwriteavar("inode:%x.%x", ip->dev, ip->inum);
    
    if(type == T_DIR){  // Create . and .. entries.
      dp->link(); // for ".."
      iupdate(dp);
      // No ip->nlink++ for ".": avoid cyclic ref count.
      if(dirlink(ip, ".", ip->inum) < 0 || dirlink(ip, "..", dp->inum) < 0)
        panic("create dots");
    }
    
    if(dirlink(dp, name, ip->inum) < 0) {
      // create race
      ip->unlink();
      iunlockput(ip);
      iput(dp, haveref);
      goto retry;
    }

    if (!dp->valid()) {
      // XXX(sbw) we need to undo everything we just did
      // (at least all the modifications to dp) and retry
      // (or return an error).
      panic("create: race");
    }
  }

  //nc_insert(dp, name, ip);
  iput(dp, haveref);
  return ip;
}

//SYSCALL
int
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
  mtreadavar("inode:%x.%x", cwd->dev, cwd->inum);

  if(omode & O_CREATE){
    if((ip = create(cwd, path, T_FILE, 0, 0)) == 0)
      return -1;
    if(omode & O_WAIT){
      // XXX wake up any open(..., O_WAIT).
      // there's a race here that's hard to fix because
      // of how non-locking create() is.
      char dummy[DIRSIZ];
      struct inode *pip = nameiparent(cwd, path, dummy);
      if(pip){
        acquire(&pip->lock);
        cv_wakeup(&pip->cv);
        release(&pip->lock);
      }
    }
    // XXX necessary because the mtwriteavar() to the same abstract variable
    // does not propagate to our scope, since create() has its own inner scope.
    mtwriteavar("inode:%x.%x", ip->dev, ip->inum);
  } else {
 retry:
    if((ip = namei(cwd, path)) == 0){
      if(omode & O_WAIT){
        char dummy[DIRSIZ];
        struct inode *pip = nameiparent(cwd, path, dummy);
        if(pip == 0)
          return -1;
        cprintf("O_WAIT waiting %s %p %d...\n", path, pip, pip->inum);
        // XXX wait for cv_wakeup above
        acquire(&pip->lock);
        cv_sleep(&pip->cv, &pip->lock);
        release(&pip->lock);
        cprintf("O_WAIT done\n");
        iput(pip);
        if(myproc()->killed == 0)
          goto retry;
      }
      return -1;
    }
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

//SYSCALL
int
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

//SYSCALL
int
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

//SYSCALL
int
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

int
doexec(const char* upath, userptr<userptr<const char> > uargv)
{
  ANON_REGION(__func__, &perfgroup);
  char *argv[MAXARG];
  char path[DIRSIZ+1];
  long r = -1;
  int i;

  if (fetchstr(path, upath, sizeof(path)) < 0)
    return -1;

  mt_ascope ascope("%s(%s)", __func__, path);

  memset(argv, 0, sizeof(argv));
  for(i=0;; i++){
    u64 uarg;    
    if(i >= NELEM(argv))
      goto clean;
    if(fetchint64(uargv+8*i, &uarg) < 0)
      goto clean;
    if(uarg == 0)
      break;

    argv[i] = (char*) kmalloc(MAXARGLEN, "execbuf");
    if (argv[i]==nullptr || fetchstr(argv[i], (char*)uarg, MAXARGLEN)<0)
      goto clean;
  }

  argv[i] = 0;
  r = exec(path, argv, &ascope);
clean:
  for (i=i-1; i >= 0; i--)
    kmfree(argv[i], MAXARGLEN);
  return r;
}

//SYSCALL
int
sys_exec(const char *upath, userptr<userptr<const char> > uargv)
{
  myproc()->data_cpuid = myid();
#if EXECSWITCH
  myproc()->uargv = uargv;
  barrier();
  // upath serves as a flag to the scheduler
  myproc()->upath = upath;
  yield();
  myproc()->upath = nullptr;
#endif
  return doexec(upath, uargv);
}

//SYSCALL
int
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

//SYSCALL
int
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

//SYSCALL
int
sys_bind(int xsock, const struct sockaddr *xaddr, int xaddrlen)
{
  extern long netbind(int, const struct sockaddr*, int);
  sref<file> f;

  if (!getsocket(xsock, &f))
    return -1;

  return netbind(f->socket, xaddr, xaddrlen);
}

//SYSCALL
int
sys_listen(int xsock, int backlog)
{
  extern long netlisten(int, int);
  sref<file> f;

  if (!getsocket(xsock, &f))
    return -1;

  return netlisten(f->socket, backlog);
}

//SYSCALL
int
sys_accept(int xsock, struct sockaddr* xaddr, u32* xaddrlen)
{
  extern long netaccept(int, struct sockaddr*, u32*);
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
