//
// File-system system calls.
// Mostly argument checking, since we don't trust
// user code, and calls into vfs_file.c and vfs_fs.c.
//

#include "types.h"
#include "defs.h"
#include "param.h"
#include "mmu.h"
#include "proc.h"
#include "fs.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "file.h"
#include "fcntl.h"
#include "cgroup.h"
#include "device.h"
#include "vfs_fs.h"
#include "mount.h"
#include "kvector.h"

// Fetch the nth word-sized system call argument as a file descriptor
// and return both the descriptor and the corresponding struct file.
static int
argfd(int n, int *pfd, struct vfs_file **pf) {
   int fd;
   struct vfs_file *f;

  if(argint(n, &fd) < 0)
    return -1;
  if(fd < 0 || fd >= NOFILE || (f=myproc()->ofile[fd]) == 0)
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
fdalloc(struct vfs_file *f)
{
  int fd;
  struct proc *curproc = myproc();

  for(fd = 0; fd < NOFILE; fd++){
    if(curproc->ofile[fd] == 0){
      curproc->ofile[fd] = f;
      return fd;
    }
  }
  return -1;
}

int
sys_dup(void)
{
  struct vfs_file *f;
  int fd;

  if(argfd(0, 0, &f) < 0)
    return -1;
  if((fd=fdalloc(f)) < 0)
    return -1;
  vfs_filedup(f);
  return fd;
}

int
sys_read(void)
{
  struct vfs_file *f;
  int n;
  char *p;  // is it a contiuous space? of what size? should we make it a vector too?

  if(argfd(0, 0, &f) < 0 || argint(2, &n) < 0 || argptr(1, &p, n) < 0)
    return -1;

  if(f->type == FD_CG){
    if(*f->cgfilename == 0)
      return cg_read(CG_DIR, f, p, n);
    else
      return cg_read(CG_FILE, f, p, n);
  }else{
    vector pv;
    pv = newvector(n,1);
    int vfs_read_result = vfs_fileread(f, n, &pv);
    memmove_from_vector(p, pv, 0, n);
    return vfs_read_result;
  } 
}

int
sys_write(void)
{
  struct vfs_file *f;
  int n;
  char *p;

  if(argfd(0, 0, &f) < 0 || argint(2, &n) < 0 || argptr(1, &p, n) < 0)
    return -1;

  if(f->type == FD_CG)
    return cg_write(f, p, n);
  else
    return vfs_filewrite(f, p, n);
}

int
sys_close(void)
{
  int fd;
  struct vfs_file *f;

  if(argfd(0, &fd, &f) < 0)
    return -1;
  myproc()->ofile[fd] = 0;
  if(f->type == FD_CG)
      cg_close(f);
  else
      vfs_fileclose(f);
  return 0;
}

int
sys_fstat(void)
{
  struct vfs_file *f;
  struct stat *st;

  if(argfd(0, 0, &f) < 0 || argptr(1, (void*)&st, sizeof(*st)) < 0)
    return -1;

  if(f->type == FD_CG)
      return cg_stat(f, st);

  return vfs_filestat(f, st);
}

// Create the path new as a link to the same inode as old.
int
sys_link(void)
{
  char name[DIRSIZ], *new, *old;
  struct vfs_inode *dp, *ip;

  if(argstr(0, &old) < 0 || argstr(1, &new) < 0)
    return -1;

  begin_op();
  if((ip = vfs_namei(old)) == 0){
    end_op();
    return -1;
  }

  ip->i_op.ilock(ip);
  if(ip->type == T_DIR){
    ip->i_op.iunlockput(ip);
    end_op();
    return -1;
  }

  ip->nlink++;
  ip->i_op.iupdate(ip);
  ip->i_op.iunlock(ip);

  if((dp = vfs_nameiparent(new, name)) == 0)
    goto bad;
  dp->i_op.ilock(dp);
  if(dp->dev != ip->dev || dp->i_op.dirlink(dp, name, ip->inum) < 0){
    dp->i_op.iunlockput(dp);
    goto bad;
  }
  dp->i_op.iunlockput(dp);
  ip->i_op.iput(ip);

  end_op();

  return 0;

bad:
  ip->i_op.ilock(ip);
  ip->nlink--;
  ip->i_op.iupdate(ip);
  ip->i_op.iunlockput(ip);
  end_op();
  return -1;
}

//PAGEBREAK!
int
sys_unlink(void)
{
  struct vfs_inode *ip, *dp;
  struct dirent de;
  char name[DIRSIZ], *path;
  uint off;

  if(argstr(0, &path) < 0)
    return -1;

  begin_op();

  int delete_cgroup_res = cgroup_delete(path, "unlink");
  if(delete_cgroup_res == -1)
  {
      // 1. search for parent derectory inode, denote it dp
      if((dp = vfs_nameiparent(path, name)) == 0){
        end_op();
        return -1;
      }

      dp->i_op.ilock(dp);

      // 2. Make sure were not trying to unlink "." or "..".
      if(vfs_namecmp(name, ".") == 0 || vfs_namecmp(name, "..") == 0)
        goto bad;

      // 3. find the inode of the unlinked file, denote it ip
      if((ip = dp->i_op.dirlookup(dp, name, &off)) == 0)
        goto bad;

      ip->i_op.ilock(ip);

      if(ip->nlink < 1)
        panic("unlink: nlink < 1");
      if(ip->type == T_DIR && !ip->i_op.isdirempty(ip)){
        ip->i_op.iunlockput(ip);
        goto bad;
      }

      if (doesbackdevice(ip) == 1) {
        ip->i_op.iunlockput(ip);
        goto bad;
      }

      // 4. erase content of the unlinked directory entry
      memset(&de, 0, sizeof(de));
      if(dp->i_op.writei(dp, (char*)&de, off, sizeof(de)) != sizeof(de))
        panic("unlink: writei");
      if(ip->type == T_DIR){
        dp->nlink--;
        dp->i_op.iupdate(dp);
      }
      dp->i_op.iunlockput(dp);

      ip->nlink--;
      ip->i_op.iupdate(ip);
      ip->i_op.iunlockput(ip);
  }
  if(delete_cgroup_res == -2){
      end_op();
      return -1;
  }

  end_op();
  return 0;

bad:
  dp->i_op.iunlockput(dp);
  end_op();
  return -1;
}

static struct vfs_inode*
createmount(char *path, short type, short major, short minor, struct mount **mnt)
{
  uint off;
  struct vfs_inode *ip, *dp;
  char name[DIRSIZ];

  if((dp = vfs_nameiparentmount(path, name, mnt)) == 0)
    return 0;
  dp->i_op.ilock(dp);

  if((ip = dp->i_op.dirlookup(dp, name, &off)) != 0){
    dp->i_op.iunlockput(dp);
    ip->i_op.ilock(ip);
    if(type == T_FILE && ip->type == T_FILE)
      return ip;
    ip->i_op.iunlockput(ip);
    mntput(*mnt);
    return 0;
  }

  if (IS_OBJ_DEVICE(dp->dev)) {
    if ((ip = obj_ialloc(dp->dev, type)) == 0)
      panic("create: obj_ialloc");
      
  } else {
    if ((ip = ialloc(dp->dev, type)) == 0)
       panic("create: ialloc");
  }

  ip->i_op.ilock(ip);
  ip->major = major;
  ip->minor = minor;
  ip->nlink = 1;
  ip->i_op.iupdate(ip);

  if(type == T_DIR){  // Create . and .. entries.
    dp->nlink++;  // for ".."
    dp->i_op.iupdate(dp);
    // No ip->nlink++ for ".": avoid cyclic ref count.
    if(ip->i_op.dirlink(ip, ".", ip->inum) < 0 || ip->i_op.dirlink(ip, "..", dp->inum) < 0)
      panic("create dots");
  }

  if(dp->i_op.dirlink(dp, name, ip->inum) < 0)
    panic("create: dirlink");

  dp->i_op.iunlockput(dp);

  return ip;
}

static struct vfs_inode*
create(char *path, short type, short major, short minor)
{
  struct mount *mnt;
  struct vfs_inode *res = createmount(path, type, major, minor, &mnt);
  if (res != 0) {
    mntput(mnt);
  }
  return res;
}

int
sys_open(void)
{
  char *path;
  int fd, omode;
  struct vfs_file *f;
  struct vfs_inode *ip;
  struct mount *mnt;

  if(argstr(0, &path) < 0 || argint(1, &omode) < 0)
    return -1;

  begin_op();

  fd = cg_sys_open(path, omode);

  if(fd >= 0){
    end_op();
    return fd;
  }

  if(omode & O_CREATE){
    ip = createmount(path, T_FILE, 0, 0, &mnt);
    if(ip == 0){
      end_op();
      return -1;
    }
  } else {
    if((ip = vfs_nameimount(path, &mnt)) == 0){
      end_op();
      return -1;
    }
    ip->i_op.ilock(ip);
    if(ip->type == T_DIR && omode != O_RDONLY){
      ip->i_op.iunlockput(ip);
      mntput(mnt);
      end_op();
      return -1;
    }
  }

  if((f = vfs_filealloc()) == 0 || (fd = fdalloc(f)) < 0){
    if(f)
      vfs_fileclose(f);
    ip->i_op.iunlockput(ip);
    mntput(mnt);
    end_op();
    return -1;
  }
  ip->i_op.iunlock(ip);
  end_op();

  f->type = FD_INODE;
  f->ip = ip;
  f->off = 0;
  f->mnt = mnt;
  f->readable = !(omode & O_WRONLY);
  f->writable = (omode & O_WRONLY) || (omode & O_RDWR);

  return fd;
}

int
sys_mkdir(void)
{
  char *path;
  struct vfs_inode *ip;

  begin_op();

  if(argstr(0, &path) < 0){
    end_op();
    return -1;
  }

  if(get_cgroup_by_path(path)){
    cprintf("cgroup already exists\n");
    end_op();
    return -1;
  }

  if(!cgroup_create(path)){
    if((ip = create(path, T_DIR, 0, 0)) == 0){
      end_op();
      return -1;
    }
    ip->i_op.iunlockput(ip);
  }
  end_op();
  return 0;
}

int
sys_mknod(void)
{
  struct vfs_inode *ip;
  char *path;
  int major, minor;

  begin_op();
  if((argstr(0, &path)) < 0 ||
     argint(1, &major) < 0 ||
     argint(2, &minor) < 0 ||
     (ip = create(path, T_DEV, major, minor)) == 0){
    end_op();
    return -1;
  }
  ip->i_op.iunlockput(ip);
  end_op();
  return 0;
}

int
sys_chdir(void)
{
  char *path;
  struct vfs_inode *ip;
  struct proc *curproc = myproc();
  struct mount *mnt;
  struct cgroup *cgp;

  begin_op();
  if(argstr(0, &path) < 0){
    end_op();
    return -1;
  }
  if((cgp = get_cgroup_by_path(path))){
    end_op();
    safestrcpy(curproc->cwdp, cgp->cgroup_dir_path, sizeof(cgp->cgroup_dir_path));
    return 0;
  }
  if((ip = vfs_nameimount(path, &mnt)) == 0){
    end_op();
    return -1;
  }
  ip->i_op.ilock(ip);
  if(ip->type != T_DIR){
    ip->i_op.iunlockput(ip);
    end_op();
    return -1;
  }
  ip->i_op.iunlock(ip);
  if(curproc->cwd)
    curproc->cwd->i_op.iput(curproc->cwd);
  if(curproc->cwdmount)
    mntput(curproc->cwdmount);
  end_op();
  curproc->cwdmount = mnt;
  curproc->cwd = ip;
  format_path(curproc->cwdp, path);
  return 0;
}

int
sys_exec(void)
{
  char *path, *argv[MAXARG];
  int i;
  uint uargv, uarg;

  if(argstr(0, &path) < 0 || argint(1, (int*)&uargv) < 0){
    return -1;
  }
  memset(argv, 0, sizeof(argv));
  for(i=0;; i++){
    if(i >= NELEM(argv))
      return -1;
    if(fetchint(uargv+4*i, (int*)&uarg) < 0)
      return -1;
    if(uarg == 0){
      argv[i] = 0;
      break;
    }
    if(fetchstr(uarg, &argv[i]) < 0)
      return -1;
  }
  return exec(path, argv);
}

int
sys_pipe(void)
{
  int *fd;
  struct vfs_file *rf, *wf;
  int fd0, fd1;

  if(argptr(0, (void*)&fd, 2*sizeof(fd[0])) < 0)
    return -1;
  if(pipealloc(&rf, &wf) < 0)
    return -1;
  fd0 = -1;
  if((fd0 = fdalloc(rf)) < 0 || (fd1 = fdalloc(wf)) < 0){
    if(fd0 >= 0)
      myproc()->ofile[fd0] = 0;
    vfs_fileclose(rf);
    vfs_fileclose(wf);
    return -1;
  }
  fd[0] = fd0;
  fd[1] = fd1;
  return 0;
}
