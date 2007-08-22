#include "types.h"
#include "stat.h"
#include "param.h"
#include "x86.h"
#include "mmu.h"
#include "proc.h"
#include "defs.h"
#include "file.h"
#include "spinlock.h"
#include "dev.h"
#include "fs.h"
#include "fsvar.h"

struct devsw devsw[NDEV];
struct spinlock file_table_lock;
struct file file[NFILE];

void
fileinit(void)
{
  initlock(&file_table_lock, "file_table");
}

// Allocate a file structure.
struct file*
filealloc(void)
{
  int i;

  acquire(&file_table_lock);
  for(i = 0; i < NFILE; i++){
    if(file[i].type == FD_CLOSED){
      file[i].type = FD_NONE;
      file[i].ref = 1;
      release(&file_table_lock);
      return file + i;
    }
  }
  release(&file_table_lock);
  return 0;
}

// Increment ref count for file f.
void
fileincref(struct file *f)
{
  acquire(&file_table_lock);
  if(f->ref < 1 || f->type == FD_CLOSED)
    panic("fileincref");
  f->ref++;
  release(&file_table_lock);
}

// Read from file f.  Addr is kernel address.
int
fileread(struct file *f, char *addr, int n)
{
  int r;
  struct inode *ip;

  if(f->readable == 0)
    return -1;
  if(f->type == FD_PIPE)
    return pipe_read(f->pipe, addr, n);
  if(f->type == FD_INODE){
    ip = ilock(f->ip);
    if((r = readi(ip, addr, f->off, n)) > 0)
      f->off += r;
    iunlock(ip);
    return r;
  }
  panic("fileread");
}

// Write to file f.  Addr is kernel address.
int
filewrite(struct file *f, char *addr, int n)
{
  int r;
  struct inode *ip;

  if(f->writable == 0)
    return -1;
  if(f->type == FD_PIPE)
    return pipe_write(f->pipe, addr, n);
  if(f->type == FD_INODE){
    ip = ilock(f->ip);
    if((r = writei(ip, addr, f->off, n)) > 0)
      f->off += r;
    iunlock(ip);
    return r;
  }
  panic("filewrite");
}

// Get metadata about file f.
int
filestat(struct file *f, struct stat *st)
{
  struct inode *ip;

  if(f->type == FD_INODE){
    ip = ilock(f->ip);
    stati(ip, st);
    iunlock(ip);
    return 0;
  }
  return -1;
}

// Close file f.  (Decrement ref count, close when reaches 0.)
void
fileclose(struct file *f)
{
  struct file ff;

  acquire(&file_table_lock);

  if(f->ref < 1 || f->type == FD_CLOSED)
    panic("fileclose");

  if(--f->ref > 0){
    release(&file_table_lock);
    return;
  }
  
  ff = *f;
  f->ref = 0;
  f->type = FD_CLOSED;
  release(&file_table_lock);
  
  if(ff.type == FD_PIPE)
    pipe_close(ff.pipe, ff.writable);
  else if(ff.type == FD_INODE)
    iput(ff.ip);
  else
    panic("fileclose");
}

