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

extern "C" {
#include "types.h"
#include "kernel.h"
#include "spinlock.h"
#include "condvar.h"
#include "buf.h"
}

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

 loop:
  // Try for cached block.
  // XXX ignore dev
  gc_begin_epoch();
  b = bufns->lookup(mkpair(dev, sector));
  if (b) {
    if (b->dev != dev || b->sector != sector)
      panic("block mismatch");
    if (*writer || !(b->flags & B_VALID)) {
      acquire(&b->lock);
      if (b->flags & B_BUSY) {
	cv_sleep(&b->cv, &b->lock);
	release(&b->lock);
	gc_end_epoch();
	goto loop;
      }

      b->flags |= B_BUSY;
      release(&b->lock);
      *writer = 1;
    }

    // rcu_end_read() happens in brelse
    return b;
  }
  gc_end_epoch();

  // Allocate fresh block.
  struct buf *victim = 0;
  bufns->enumerate([&victim](const pair<u32, u64>&, buf *b)->bool {
      acquire(&b->lock);
      if ((b->flags & (B_BUSY | B_DIRTY | B_VALID)) == 0) {
        victim = b;
        return true;
      }
      release(&b->lock);
      return false;
    });
  if (victim == 0)
    bufns->enumerate([&victim](const pair<u32, u64>&, buf *b)->bool {
        acquire(&b->lock);
        if ((b->flags & (B_BUSY | B_DIRTY)) == 0) {
          victim = b;
          return true;
        }
        release(&b->lock);
        return false;
      });
  if (victim == 0)
    panic("bget all busy");
  victim->flags |= B_BUSY;
  bufns->remove(mkpair(victim->dev, victim->sector), &victim);
  release(&victim->lock);
  destroylock(&victim->lock);
  gc_delayed(victim, kmfree);

  b = (buf*) kmalloc(sizeof(*b));
  b->dev = dev;
  b->sector = sector;
  b->flags = B_BUSY;
  *writer = 1;
  snprintf(b->lockname, sizeof(b->lockname), "cv:buf:%d", b->sector);
  initlock(&b->lock, b->lockname+3, LOCKSTAT_BIO);
  initcondvar(&b->cv, b->lockname);
  gc_begin_epoch();
  if (bufns->insert(mkpair(b->dev, b->sector), b) < 0) {
    destroylock(&b->lock);
    gc_delayed(b, kmfree);
    goto loop;
  }
  // rcu_end_read() happens in brelse
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
    __sync_fetch_and_and(&b->flags, ~B_BUSY);
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
    __sync_fetch_and_and(&b->flags, ~B_BUSY);
    cv_wakeup(&b->cv);
  }
  // rcu_begin_read() happens in bread
  gc_end_epoch();
}

void
initbio(void)
{
  bufns = new xns<pair<u32, u64>, buf*, bio_hash>(false);

  for (u64 i = 0; i < NBUF; i++) {
    struct buf *b = (buf*) kmalloc(sizeof(*b));
    b->dev = 0xdeadbeef;
    b->sector = -i;	/* dummy to pre-allocate NBUF spaces for evict */
    b->flags = 0;
    initlock(&b->lock, "bcache-lock", LOCKSTAT_BIO);
    initcondvar(&b->cv, "bcache-cv");
    if (bufns->insert(mkpair(b->dev, b->sector), b) < 0)
      panic("binit ns_insert");
  }
}
