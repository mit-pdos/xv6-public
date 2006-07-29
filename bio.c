#include "types.h"
#include "param.h"
#include "x86.h"
#include "mmu.h"
#include "proc.h"
#include "defs.h"
#include "spinlock.h"
#include "buf.h"

struct buf buf[NBUF];
struct spinlock buf_table_lock = { "buf_table" };

struct buf *
getblk()
{
  int i;

  acquire(&buf_table_lock);

  // XXX need to lock the block even if not caching, to
  // avoid read modify write problems.

  while(1){
    for(i = 0; i < NBUF; i++){
      if((buf[i].flags & B_BUSY) == 0){
        buf[i].flags |= B_BUSY;
        release(&buf_table_lock);
        return buf + i;
      }
    }
    sleep(buf, &buf_table_lock);
  }
}

struct buf *
bread(uint dev, uint sector)
{
  void *c;
  struct buf *b;
  extern struct spinlock ide_lock;

  b = getblk();

  acquire(&ide_lock);
  c = ide_start_read(dev & 0xff, sector, b->data, 1);
  // sleep (c, &ide_lock);
  ide_finish_read(c);
  release(&ide_lock);

  return b;
}

void
brelse(struct buf *b)
{
  if((b->flags & B_BUSY) == 0)
    panic("brelse");
  
  acquire(&buf_table_lock);

  b->flags &= ~B_BUSY;
  wakeup(buf);

  release(&buf_table_lock);
}

