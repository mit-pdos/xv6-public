// Buffer cache.
//
// The buffer cache is a linked list of buf structures holding
// cached copies of disk block contents.  Caching disk blocks
// in memory reduces the number of disk reads and also provides
// a synchronization point for disk blocks used by multiple processes.
//
// Interface:
// * To get a buffer for a particular disk block, call bread.
// * After changing buffer data, call bwrite to write it to disk.
// * When done with the buffer, call brelse.
// * Do not use the buffer after calling brelse.
// * Only one process at a time can use a buffer,
//     so do not keep them longer than necessary.
//
// The implementation uses two state flags internally:
// * B_VALID: the buffer data has been read from the disk.
// * B_DIRTY: the buffer data has been modified
//     and needs to be written to disk.

#include "types.h"
#include "defs.h"
#include "param.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "fs.h"
#include "buf.h"
#include "device.h"
#include "vfs_fs.h"
#include "file.h"
#include "vfs_file.h"
#include "proc.h"
#include "cgroup.h"
#include "kvector.h"

struct {
  struct spinlock lock;
  struct buf buf[NBUF];

  // Linked list of all buffers, through prev/next.
  // head.next is most recently used.
  struct buf head;
} bcache;

void
binit(void)
{
  struct buf *b;

  initlock(&bcache.lock, "bcache");

//PAGEBREAK!
  // Create linked list of buffers
  bcache.head.prev = &bcache.head;
  bcache.head.next = &bcache.head;
  for(b = bcache.buf; b < bcache.buf+NBUF; b++){
    b->next = bcache.head.next;
    b->prev = &bcache.head;
    initsleeplock(&b->lock, "buffer");
    bcache.head.next->prev = b;
    bcache.head.next = b;
  }
}

void
invalidateblocks(uint dev)
{
  acquire(&bcache.lock);
  struct buf *b;
  for(b = bcache.head.next; b != &bcache.head; b = b->next){
    if(b->dev == dev){
      b->flags &= ~(B_VALID|B_DIRTY);
    }
  }
  release(&bcache.lock);
}

// Look through buffer cache for block on device dev.
// If not found, allocate a buffer.
// In either case, return locked buffer.
static struct buf*
bget(uint dev, uint blockno)
{
  struct buf *b;
  struct cgroup *cg = proc_get_cgroup();

  acquire(&bcache.lock);

  // Is the block already cached?
  for(b = bcache.head.next; b != &bcache.head; b = b->next){
    if(b->dev == dev && b->blockno == blockno){
      b->refcnt++;
      release(&bcache.lock);
      acquiresleep(&b->lock);
      cgroup_mem_stat_pgfault_incr(cg);
      return b;
    }
  }

  // Not cached; recycle an unused buffer.
  // Even if refcnt==0, B_DIRTY indicates a buffer is in use
  // because log.c has modified it but not yet committed it.
  for(b = bcache.head.prev; b != &bcache.head; b = b->prev){
    if(b->refcnt == 0 && (b->flags & B_DIRTY) == 0) {
      b->dev = dev;
      b->blockno = blockno;
      b->flags = 0;
      b->cgroup = 0;
      b->refcnt = 1;
      release(&bcache.lock);
      acquiresleep(&b->lock);
      cgroup_mem_stat_pgmajfault_incr(cg);
      return b;
    }
  }
  panic("bget: no buffers");
}

void
devicerw(struct inode *device, struct buf *b)
{
  if ((b->flags & B_DIRTY) == 0) {
      vector read_result_vector;
      read_result_vector = newvector(BSIZE, 1);
      device->vfs_inode.i_op.readi(&device->vfs_inode, BSIZE*b->blockno, BSIZE, &read_result_vector);
      memmove_from_vector((char*)b->data, read_result_vector, 0, BSIZE);
      // debugging: vectormemcmp("devicerw", read_result_vector, 0, (char *) b->data, BSIZE);
      freevector(&read_result_vector);
  } else {
      device->vfs_inode.i_op.writei(&device->vfs_inode, (char *) b->data, BSIZE*b->blockno, BSIZE);
  }
  b->flags |= B_VALID;
  b->flags &= ~B_DIRTY;
}

void
brw(struct buf *b)
{
  struct vfs_inode *device;
  if ((device = getinodefordevice(b->dev)) != 0) {
      struct inode * i_device = container_of(device, struct inode, vfs_inode);

      devicerw(i_device, b);
  } else {
      iderw(b);
  }
}

// Return a locked buf with the contents of the indicated block.
struct buf*
bread(uint dev, uint blockno)
{
  struct buf *b;

  b = bget(dev, blockno);
  if((b->flags & B_VALID) == 0) {
    brw(b);
  }
  return b;
}

// Write b's contents to disk.  Must be locked.
void
bwrite(struct buf *b)
{
  if(!holdingsleep(&b->lock))
    panic("bwrite");
  b->flags |= B_DIRTY;
  brw(b);
}

// Release a locked buffer.
// Move to the head of the MRU list.
void
brelse(struct buf *b)
{
  if(!holdingsleep(&b->lock))
    panic("brelse");

  releasesleep(&b->lock);

  acquire(&bcache.lock);
  b->refcnt--;
  if (b->refcnt == 0) {
    // no one is waiting for it.
    b->next->prev = b->prev;
    b->prev->next = b->next;
    b->next = bcache.head.next;
    b->prev = &bcache.head;
    bcache.head.next->prev = b;
    bcache.head.next = b;
  }

  release(&bcache.lock);
}
//PAGEBREAK!
// Blank page.
