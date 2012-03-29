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
// The implementation uses three state flags internally:
// * B_BUSY: the block has been returned from bread
//     and has not been passed back to brelse.  
// * B_VALID: the buffer data has been initialized
//     with the associated disk block contents.
// * B_DIRTY: the buffer data has been modified
//     and needs to be written to disk.

#include "types.h"
#include "kernel.hh"
#include "spinlock.h"
#include "condvar.h"
#include "buf.hh"
#include "cpputil.hh"
#include "ns.hh"

u64
bio_hash(const pair<u32, u64> &p)
{
  return p._a ^ p._b;
}

static xns<pair<u32, u64>, buf*, bio_hash> *bufns;

enum { writeback = 0 };

// Look through buffer cache for sector on device dev.
// If not found, allocate fresh block.
// In either case, return locked buffer.
static struct buf*
bget(u32 dev, u64 sector, int *writer)
{
  struct buf *b;
  scoped_gc_epoch e;

 loop:
  // Try for cached block.
  // XXX ignore dev
  b = bufns->lookup(mkpair(dev, sector));
  if (b) {
    if (b->dev != dev || b->sector != sector)
      panic("block mismatch");
    if (*writer || !(b->flags & B_VALID)) {
      acquire(&b->lock);
      if (b->flags & B_BUSY) {
	cv_sleep(&b->cv, &b->lock);
	release(&b->lock);
	goto loop;
      }

      b->flags |= B_BUSY;
      release(&b->lock);
      *writer = 1;
    }

    // rcu_end_read() happens in brelse
    return b;
  }

  // Allocate fresh block.
  b = new buf(dev, sector);
  b->flags = B_BUSY;
  *writer = 1;
  if (bufns->insert(mkpair(b->dev, b->sector), b) < 0) {
    gc_delayed(b);
    goto loop;
  }
  return b;
}

// Return a B_BUSY buf with the contents of the indicated disk sector.
struct buf*
bread(u32 dev, u64 sector, int writer)
{
  struct buf *b;

  int origwriter = writer;
  b = bget(dev, sector, &writer);
  if(!(b->flags & B_VALID))
    iderw(b);
  if (writer && !origwriter) {
    b->flags &= ~B_BUSY;
    cv_wakeup(&b->cv);
  }
  return b;
}

// Write b's contents to disk.  Must be locked.
void
bwrite(struct buf *b)
{
  if((b->flags & B_BUSY) == 0)
    panic("bwrite");
  b->flags |= B_DIRTY;
  if (writeback)
    iderw(b);
}

// Release the buffer b.
void
brelse(struct buf *b, int writer)
{
  if (writer) {
    if((b->flags & B_BUSY) == 0)
      panic("brelse");
    b->flags &= ~B_BUSY;
    cv_wakeup(&b->cv);
  }
}

void
initbio(void)
{
  bufns = new xns<pair<u32, u64>, buf*, bio_hash>(false);
}
