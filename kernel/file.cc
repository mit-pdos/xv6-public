#include "types.h"
#include "kernel.hh"
#include "spinlock.h"
#include "condvar.h"
#include "fs.h"
#include "file.hh"
#include "stat.h"
#include "net.hh"

struct devsw __mpalign__ devsw[NDEV];

file*
file::alloc(void)
{
  return new file();
}

file::file(void)
  : rcu_freed("file"), 
    type(file::FD_NONE), readable(0), writable(0), 
    socket(0), pipe(nullptr), ip(nullptr), off(0)
{
}

void
file::onzero(void) const
{
  if(type == file::FD_PIPE)
    pipeclose(pipe, writable);
  else if(type == file::FD_INODE)
    iput(ip);
  else if(type == file::FD_SOCKET)
    netclose(socket);
  else if(type != file::FD_NONE)
    panic("file::close bad type");
  gc_delayed((file*)this);
}

void
file::do_gc(void)
{
  delete this;
}

file*
file::dup(void)
{
  inc();
  return this;
}

int
file::stat(struct stat *st)
{
  if(type == file::FD_INODE){
    ilock(ip, 0);
    if(ip->type == 0)
      panic("filestat");
    stati(ip, st);
    iunlock(ip);
    return 0;
  }
  return -1;
}

int
file::read(char *addr, int n)
{
  int r;

  if(readable == 0)
    return -1;
  if(type == file::FD_PIPE)
    return piperead(pipe, addr, n);
  if(type == file::FD_INODE){
    ilock(ip, 0);
    if(ip->type == 0)
      panic("fileread");
    if((r = readi(ip, addr, off, n)) > 0)
      off += r;
    iunlock(ip);
    return r;
  }
  if(type == file::FD_SOCKET)
    return netread(socket, addr, n);
  panic("fileread");
}

ssize_t
file::pread(char *addr, size_t n, off_t off)
{
  if(type == file::FD_INODE){
    int r;
    if(ip->type == 0)
      panic("file::pread");
    r = readi(ip, addr, off, n);
    return r;
  }
  return -1;
}

int
file::write(const char *addr, int n)
{
  int r;

  if(writable == 0)
    return -1;
  if(type == file::FD_PIPE)
    return pipewrite(pipe, addr, n);
  if(type == file::FD_INODE){
    ilock(ip, 1);
    if(ip->type == 0 || ip->type == T_DIR)
      panic("filewrite but 0 or T_DIR");
    if((r = writei(ip, addr, off, n)) > 0)
      off += r;
    iunlock(ip);
    return r;
  }
  if(type == file::FD_SOCKET)
    return netwrite(socket, addr, n);
  panic("filewrite");
}
