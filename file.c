#include "types.h"
#include "param.h"
#include "spinlock.h"
#include "condvar.h"
#include "kernel.h"
#include "fs.h"
#include "file.h"
#include "stat.h"

struct devsw __mpalign__ devsw[NDEV];

// Allocate a file structure.
struct file*
filealloc(void)
{
  struct file *f = kmalloc(sizeof(struct file));
  f->ref = 1;
  return f;
}

// Increment ref count for file f.
struct file*
filedup(struct file *f)
{
  if (__sync_fetch_and_add(&f->ref, 1) < 1)
    panic("filedup");
  return f;
}

// Close file f.  (Decrement ref count, close when reaches 0.)
void
fileclose(struct file *f)
{
  if (__sync_sub_and_fetch(&f->ref, 1) > 0)
    return;

  if(f->type == FD_PIPE)
    pipeclose(f->pipe, f->writable);
  else if(f->type == FD_INODE)
    iput(f->ip);
  kmfree(f);
}

// Get metadata about file f.
int
filestat(struct file *f, struct stat *st)
{
  if(f->type == FD_INODE){
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
  if(f->type == FD_PIPE)
    return piperead(f->pipe, addr, n);
  if(f->type == FD_INODE){
    ilock(f->ip, 0);
    if(f->ip->type == 0)
      panic("fileread");
    if((r = readi(f->ip, addr, f->off, n)) > 0)
      f->off += r;
    iunlock(f->ip);
    return r;
  }
  panic("fileread");
}

// Write to file f.  Addr is kernel address.
int
filewrite(struct file *f, char *addr, int n)
{
  int r;

  if(f->writable == 0)
    return -1;
  if(f->type == FD_PIPE)
    return pipewrite(f->pipe, addr, n);
  if(f->type == FD_INODE){
    ilock(f->ip, 1);
    if(f->ip->type == 0 || f->ip->type == T_DIR)
      panic("filewrite but 0 or T_DIR");
    if((r = writei(f->ip, addr, f->off, n)) > 0)
      f->off += r;
    iunlock(f->ip);
    return r;
  }
  panic("filewrite");
}

