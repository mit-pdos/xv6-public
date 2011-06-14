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
#include "defs.h"
#include "param.h"
#include "spinlock.h"
#include "condvar.h"
#include "buf.h"

static struct ns *bufns;

void
binit(void)
{
  bufns = nsalloc(0);

  for (uint i = 0; i < NBUF; i++) {
    struct buf *b = kmalloc(sizeof(*b));
    b->dev = 0xdeadbeef;
    b->sector = -i;	/* dummy to pre-allocate NBUF spaces for evict */
    b->flags = 0;
    initlock(&b->lock, "bcache-lock");
    initcondvar(&b->cv, "bcache-cv");
    if (ns_insert(bufns, b->sector, b) < 0)
      panic("binit ns_insert");
  }
}

static void *
evict(uint key, void *bp)
{
  struct buf *b = bp;
  acquire(&b->lock);
  if ((b->flags & (B_BUSY | B_VALID)) == 0)
    return b;
  release(&b->lock);
  return 0;
}

static void *
evict_valid(uint key, void *bp)
{
  struct buf *b = bp;
  acquire(&b->lock);
  if ((b->flags & B_BUSY) == 0)
    return b;
  release(&b->lock);
  return 0;
}

// Look through buffer cache for sector on device dev.
// If not found, allocate fresh block.
// In either case, return locked buffer.
static struct buf*
bget(uint dev, uint sector)
{
  struct buf *b;

 loop:
  // Try for cached block.
  // XXX ignore dev
  rcu_begin_read();
  b = ns_lookup(bufns, sector);
  if (b) {
    acquire(&b->lock);
    if (b->dev != dev)
      panic("dev mismatch");
    if (!(b->flags & B_BUSY)) {
      b->flags |= B_BUSY;
      release(&b->lock);
      rcu_end_read();
      return b;
    }

    cv_sleep(&b->cv, &b->lock);
    release(&b->lock);
    rcu_end_read();
    goto loop;
  }
  rcu_end_read();

  // Allocate fresh block.
  struct buf *victim = ns_enumerate(bufns, evict);
  if (victim == 0)
    victim = ns_enumerate(bufns, evict_valid);
  if (victim == 0)
    panic("bget all busy");
  victim->flags |= B_BUSY;
  ns_remove(bufns, victim->sector, victim);
  release(&victim->lock);
  rcu_delayed(victim, kmfree);

  b = kmalloc(sizeof(*b));
  b->dev = dev;
  b->sector = sector;
  b->flags = B_BUSY;
  initlock(&b->lock, "bcache-lock");
  initcondvar(&b->cv, "bcache-cv");
  if (ns_insert(bufns, b->sector, b) < 0) {
    rcu_delayed(b, kmfree);
    goto loop;
  }
  return b;
}

// Return a B_BUSY buf with the contents of the indicated disk sector.
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
  if((b->flags & B_BUSY) == 0)
    panic("bwrite");
  b->flags |= B_DIRTY;
  iderw(b);
}

// Release the buffer b.
void
brelse(struct buf *b)
{
  acquire(&b->lock);
  if((b->flags & B_BUSY) == 0)
    panic("brelse");
  b->flags &= ~B_BUSY;
  release(&b->lock);
  cv_wakeup(&b->cv);
}

