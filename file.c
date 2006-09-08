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

struct spinlock file_table_lock;
struct devsw devsw[NDEV];

struct file file[NFILE];

void
fileinit(void)
{
  initlock(&file_table_lock, "file_table");
}

// Allocate a file structure
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

// Write to file f.  Addr is kernel address.
int
filewrite(struct file *f, char *addr, int n)
{
  if(f->writable == 0)
    return -1;
  if(f->type == FD_PIPE){
    return pipe_write(f->pipe, addr, n);
  } else if(f->type == FD_FILE) {
    ilock(f->ip);
    int r = writei(f->ip, addr, f->off, n);
    if(r > 0) {
      f->off += r;
    }
    iunlock(f->ip);
    return r;
  } else {
    panic("filewrite");
    return -1;
  }
}

// Read from file f.  Addr is kernel address.
int
fileread(struct file *f, char *addr, int n)
{
  if(f->readable == 0)
    return -1;
  if(f->type == FD_PIPE){
    return pipe_read(f->pipe, addr, n);
  } else if(f->type == FD_FILE){
    ilock(f->ip);
    int cc = readi(f->ip, addr, f->off, n);
    if(cc > 0)
      f->off += cc;
    iunlock(f->ip);
    return cc;
  } else {
    panic("fileread");
    return -1;
  }
}

// Close file f.  (Decrement ref count, close when reaches 0.)
void
fileclose(struct file *f)
{
  acquire(&file_table_lock);

  if(f->ref < 1 || f->type == FD_CLOSED)
    panic("fileclose");

  if(--f->ref == 0){
    struct file dummy = *f;

    f->ref = 0;
    f->type = FD_CLOSED;
    release(&file_table_lock);

    if(dummy.type == FD_PIPE){
      pipe_close(dummy.pipe, dummy.writable);
    } else if(dummy.type == FD_FILE){
      idecref(dummy.ip);
    } else {
      panic("fileclose");
    }
  } else {
    release(&file_table_lock);
  }
}

// Get metadata about file f.
int
filestat(struct file *f, struct stat *st)
{
  if(f->type == FD_FILE){
    ilock(f->ip);
    stati(f->ip, st);
    iunlock(f->ip);
    return 0;
  } else
    return -1;
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
