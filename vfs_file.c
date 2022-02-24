//
// File descriptors
//

#include "types.h"
#include "defs.h"
#include "param.h"
#include "fs.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "file.h"
#include "vfs_file.h"
#include "device.h"

struct devsw devsw[NDEV];

void
vfs_fileinit(void)
{
  initlock(&ftable.lock, "ftable");
}

// Allocate a file structure.
struct vfs_file*
vfs_filealloc(void)
{
  struct vfs_file *f;

  acquire(&ftable.lock);
  for(f = ftable.file; f < ftable.file + NFILE; f++){
    if(f->ref == 0){
      f->ref = 1;
      release(&ftable.lock);
      return f;
    }
  }
  release(&ftable.lock);
  return 0;
}

// Increment ref count for file f.
struct vfs_file*
vfs_filedup(struct vfs_file *f)
{
  acquire(&ftable.lock);
  if(f->ref < 1)
    panic("vfs_filedup");
  f->ref++;
  release(&ftable.lock);
    return f;
}

// Close file f.  (Decrement ref count, close when reaches 0.)
void
vfs_fileclose(struct vfs_file *f)
{
    struct vfs_file ff;

    acquire(&ftable.lock);
    if(f->ref < 1)
        panic("vfs_fileclose");
    if(--f->ref > 0){
        release(&ftable.lock);
        return;
    }
    ff = *f;
    f->ref = 0;
    f->type = FD_NONE;
    release(&ftable.lock);

    if(ff.type == FD_PIPE) {
        pipeclose(ff.pipe, ff.writable);
    }
    else if(ff.type == FD_INODE){
        mntput(ff.mnt);
        begin_op();
        ff.ip->i_op.iput(ff.ip);
        end_op();
    }
}

// Get metadata about file f.
int
vfs_filestat(struct vfs_file *f, struct stat *st)
{
  if(f->type == FD_INODE){
    f->ip->i_op.ilock(f->ip);
    f->ip->i_op.stati(f->ip, st);
    f->ip->i_op.iunlock(f->ip);
    return 0;
  }
  return -1;
}

// Read from file f.
int
vfs_fileread(struct vfs_file *f, int n, vector * dstvector)
{
  int r;

  if(f->readable == 0)
    return -1;
  if(f->type == FD_PIPE){
    int piperesult = piperead(f->pipe, n, dstvector);
    return piperesult;
  }
  if(f->type == FD_INODE){
    f->ip->i_op.ilock(f->ip);
    if((r = f->ip->i_op.readi(f->ip, f->off, n, dstvector)) > 0) {
        f->off += r;
    }
    f->ip->i_op.iunlock(f->ip);
    return r;
  }

  panic("vfs_fileread");
}

//PAGEBREAK!
// Write to file f.
int
vfs_filewrite(struct vfs_file *f, char *addr, int n)
{
  int r;

  if(f->writable == 0)
    return -1;
  if(f->type == FD_PIPE)
    return pipewrite(f->pipe, addr, n);
  if(f->type == FD_INODE){
    // write a few blocks at a time to avoid exceeding
    // the maximum log transaction size, including
    // i-node, indirect block, allocation blocks,
    // and 2 blocks of slop for non-aligned writes.
    // this really belongs lower down, since writei()
    // might be writing a device like the console.
    int max = ((MAXOPBLOCKS-1-1-2) / 2) * 512;
    int i = 0;
    while(i < n){
      int n1 = n - i;
      if(n1 > max)
        n1 = max;
        
      begin_op();
      f->ip->i_op.ilock(f->ip);
      if ((r = f->ip->i_op.writei(f->ip, addr + i, f->off, n1)) > 0)
        f->off += r;
      f->ip->i_op.iunlock(f->ip);
      end_op();

      if(r < 0)
        break;
      if(r != n1)
        panic("short vfs_filewrite");
      i += r;
    }
    return i == n ? n : -1;
  }
  panic("vfs_filewrite");
}

