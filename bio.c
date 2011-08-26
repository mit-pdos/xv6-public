// Buffer cache.
//
// The buffer cache is a linked list of buf structures holding
// cached copies of disk block contents.  Caching disk blocks
// in memory reduces the number of disk reads and also provides
// a synchronization point for disk blocks used by multiple processes.
// 
// Interface:
// * To get a buffer for a particular disk block, call bread.
// * After changing buffer data, call bwrite to flush it to disk.
// * When done with the buffer, call brelse.
// * Do not use the buffer after calling brelse.
// * Only one process at a time can use a buffer,
//     so do not keep them longer than necessary.
// 
// The implementation uses two state flags internally:
// * B_VALID: the buffer data has been initialized
//     with the associated disk block contents.
// * B_DIRTY: the buffer data has been modified
//     and needs to be written to disk.

#include "types.h"
#include "defs.h"
#include "param.h"
#include "spinlock.h"
#include "buf.h"

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
    b->dev = -1;
    initlock(&b->lock, "buf");
    initsleeplock(&b->sleeplock);
    bcache.head.next->prev = b;
    bcache.head.next = b;
  }
}

// Look through buffer cache for sector on device dev.
// If not found, allocate fresh block.
// In either case, return sleep-locked buffer.
static struct buf*
bget(uint dev, uint sector)
{
  struct buf *b;

  acquire(&bcache.lock);
  // Try for cached block.
  for(b = bcache.head.next; b != &bcache.head; b = b->next){
    acquire(&b->lock);
    if(b->dev == dev && b->sector == sector){
      release(&bcache.lock);
      acquire_sleeplock(&b->sleeplock, &b->lock);
      release(&b->lock);
      return b;
    }
    release(&b->lock);
  }

  // Allocate fresh block.
  for(b = bcache.head.prev; b != &bcache.head; b = b->prev){
    acquire(&b->lock);
    if (!acquired_sleeplock(&b->sleeplock)) {
      release(&bcache.lock);
      b->dev = dev;
      b->sector = sector;
      b->flags = 0;
      acquire_sleeplock(&b->sleeplock, &b->lock);
      release(&b->lock);
      return b;
    }
    release(&b->lock);
  }
  panic("bget: no buffers");
}

// Return a locked buf with the contents of the indicated disk sector.
struct buf*
bread(uint dev, uint sector)
{
  struct buf *b;

  b = bget(dev, sector);
  if(!(b->flags & B_VALID))
    iderw(b);
  return b;
}

// Write b's contents to disk.  Must be locked.
void
bwrite(struct buf *b)
{
  if(!acquired_sleeplock(&b->sleeplock))
    panic("bwrite");
  b->flags |= B_DIRTY;
  iderw(b);
}

// Release the buffer b.
void
brelse(struct buf *b)
{
  if(!acquired_sleeplock(&b->sleeplock))
    panic("brelse");

  acquire(&bcache.lock);
  acquire(&b->lock);
  b->next->prev = b->prev;
  b->prev->next = b->next;
  b->next = bcache.head.next;
  b->prev = &bcache.head;
  bcache.head.next->prev = b;
  bcache.head.next = b;

  release_sleeplock(&b->sleeplock);
  release(&b->lock);

  release(&bcache.lock);
}

