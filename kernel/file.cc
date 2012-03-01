#include "types.h"
#include "kernel.hh"
#include "spinlock.h"
#include "condvar.h"
#include "fs.h"
#include "file.hh"
#include "stat.h"
#include "net.hh"

struct devsw __mpalign__ devsw[NDEV];

// Allocate a file structure.
struct file*
filealloc(void)
{
  struct file *f = (file*) kmalloc(sizeof(struct file));
  f->ref = 1;
  f->type = file::FD_NONE;
  return f;
}

// Increment ref count for file f.
struct file*
filedup(struct file *f)
{
  if (f->ref++ < 1)
    panic("filedup");
  return f;
}

// Close file f.  (Decrement ref count, close when reaches 0.)
void
fileclose(struct file *f)
{
  if (--f->ref > 0)
    return;

  if(f->type == file::FD_PIPE)
    pipeclose(f->pipe, f->writable);
  else if(f->type == file::FD_INODE)
    iput(f->ip);
  else if(f->type == file::FD_SOCKET)
    netclose(f->socket);
  else if(f->type != file::FD_NONE)
    panic("fileclose bad type");
  kmfree(f, sizeof(struct file));
}

// Get metadata about file f.
int
filestat(struct file *f, struct stat *st)
{
  if(f->type == file::FD_INODE){
    ilock(f->ip, 0);
    if(f->ip->type == 0)
      panic("filestat");
    stati(f->ip, st);
    iunlock(f->ip);
    return 0;
  }
  return -1;
}

// Read from file f.  Addr is kernel address.
int
fileread(struct file *f, char *addr, int n)
{
  int r;

  if(f->readable == 0)
    return -1;
  if(f->type == file::FD_PIPE)
    return piperead(f->pipe, addr, n);
  if(f->type == file::FD_INODE){
    ilock(f->ip, 0);
    if(f->ip->type == 0)
      panic("fileread");
    if((r = readi(f->ip, addr, f->off, n)) > 0)
      f->off += r;
    iunlock(f->ip);
    return r;
  }
  if(f->type == file::FD_SOCKET)
    return netread(f->socket, addr, n);
  panic("fileread");
}

// Write to file f.  Addr is kernel address.
int
filewrite(struct file *f, char *addr, int n)
{
  int r;

  if(f->writable == 0)
    return -1;
  if(f->type == file::FD_PIPE)
    return pipewrite(f->pipe, addr, n);
  if(f->type == file::FD_INODE){
    ilock(f->ip, 1);
    if(f->ip->type == 0 || f->ip->type == T_DIR)
      panic("filewrite but 0 or T_DIR");
    if((r = writei(f->ip, addr, f->off, n)) > 0)
      f->off += r;
    iunlock(f->ip);
    return r;
  }
  if(f->type == file::FD_SOCKET)
    return netwrite(f->socket, addr, n);
  panic("filewrite");
}
