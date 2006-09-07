// Buffer cache.
//
// The buffer cache is a linked list of buf structures
// holding cached copies of disk block contents.
// Each buf has two state bits B_BUSY and B_VALID.
// If B_BUSY is set, it means that some code is currently
// editing buf, so other code is not allowed to look at it.
// To wait for a buffer that is B_BUSY, sleep on buf.
// (See bget below.)
// 
// If B_VALID is set, it means that the memory contents
// have been initialized by reading them off the disk.
// (Conversely, if B_VALID is not set, the memory contents
// of buf must be initialized, often by calling bread,
// before being used.)
// 
// After making changes to a buf's memory, call bwrite to flush
// the changes out to disk, to keep the disk and memory copies
// in sync.
//
// When finished with a buffer, call brelse to release the buffer
// (i.e., clear B_BUSY), so that others can access it.
//
// Bufs that are not B_BUSY are fair game for reuse for other
// disk blocks.  It is not allowed to use a buf after calling brelse.

#include "types.h"
#include "param.h"
#include "x86.h"
#include "mmu.h"
#include "proc.h"
#include "defs.h"
#include "spinlock.h"
#include "buf.h"

struct buf buf[NBUF];
struct spinlock buf_table_lock;

// Linked list of all buffers, through prev/next.
// bufhead->next is most recently used.
// bufhead->tail is least recently used.
struct buf bufhead;

void
binit(void)
{
  struct buf *b;

  initlock(&buf_table_lock, "buf_table");

  // Create linked list of buffers
  bufhead.prev = &bufhead;
  bufhead.next = &bufhead;
  for(b = buf; b < buf+NBUF; b++){
    b->next = bufhead.next;
    b->prev = &bufhead;
    bufhead.next->prev = b;
    bufhead.next = b;
  }
}

// Look through buffer cache for block n on device dev.
// If not found, allocate fresh block.
// In either case, return locked buffer.
static struct buf*
bget(uint dev, uint sector)
{
  struct buf *b;

  acquire(&buf_table_lock);

  for(;;){
    for(b = bufhead.next; b != &bufhead; b = b->next)
      if((b->flags & (B_BUSY|B_VALID)) &&
         b->dev == dev && b->sector == sector)
        break;

    if(b != &bufhead){
      if(b->flags & B_BUSY){
        sleep(buf, &buf_table_lock);
      } else {
        b->flags |= B_BUSY;
        // b->flags &= ~B_VALID; // Force reread from disk
        release(&buf_table_lock);
        return b;
      }
    } else {
      for(b = bufhead.prev; b != &bufhead; b = b->prev){
        if((b->flags & B_BUSY) == 0){
          b->flags = B_BUSY;
          b->dev = dev;
          b->sector = sector;
          release(&buf_table_lock);
          return b;
        }
      }
      panic("bget: no buffers");
    }
  }
}

// Read buf's contents from disk.
struct buf*
bread(uint dev, uint sector)
{
  struct buf *b;

  b = bget(dev, sector);
  if(b->flags & B_VALID)
    return b;

  ide_rw(dev & 0xff, sector, b->data, 1, 1);
  b->flags |= B_VALID;

  return b;
}

// Write buf's contents to disk.
// Must be locked.
void
bwrite(struct buf *b, uint sector)
{
  if((b->flags & B_BUSY) == 0)
    panic("bwrite");

  ide_rw(b->dev & 0xff, sector, b->data, 1, 0);
  b->flags |= B_VALID;
}

// Release the buffer buf.
void
brelse(struct buf *b)
{
  if((b->flags & B_BUSY) == 0)
    panic("brelse");

  acquire(&buf_table_lock);

  b->next->prev = b->prev;
  b->prev->next = b->next;
  b->next = bufhead.next;
  b->prev = &bufhead;
  bufhead.next->prev = b;
  bufhead.next = b;

  b->flags &= ~B_BUSY;
  wakeup(buf);

  release(&buf_table_lock);
}

