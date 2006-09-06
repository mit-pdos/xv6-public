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

// linked list of all buffers, through prev/next.
// bufhead->next is most recently used.
// bufhead->tail is least recently used.
struct buf bufhead;

void
binit(void)
{
  struct buf *b;

  initlock(&buf_table_lock, "buf_table");

  bufhead.prev = &bufhead;
  bufhead.next = &bufhead;
  for(b = buf; b < buf+NBUF; b++){
    b->next = bufhead.next;
    b->prev = &bufhead;
    bufhead.next->prev = b;
    bufhead.next = b;
  }
}

struct buf*
getblk(uint dev, uint sector)
{
  struct buf *b;

  acquire(&buf_table_lock);

  for(;;){
    for(b = bufhead.next; b != &bufhead; b = b->next)
      if((b->flags & (B_BUSY|B_VALID)) && b->dev == dev && b->sector == sector)
        break;

    if(b != &bufhead){
      if(b->flags & B_BUSY){
        sleep(buf, &buf_table_lock);
      } else {
        b->flags |= B_BUSY;
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
      panic("getblk: no buffers");
    }
  }
}

struct buf*
bread(uint dev, uint sector)
{
  void *c;
  struct buf *b;
  extern struct spinlock ide_lock;

  b = getblk(dev, sector);
  if(b->flags & B_VALID)
    return b;

  acquire(&ide_lock);
  c = ide_start_rw(dev & 0xff, sector, b->data, 1, 1);
  sleep(c, &ide_lock);
  ide_finish(c);
  b->flags |= B_VALID;
  release(&ide_lock);

  return b;
}

void
bwrite(struct buf *b, uint sector)
{
  void *c;
  extern struct spinlock ide_lock;

  acquire(&ide_lock);
  c = ide_start_rw(b->dev & 0xff, sector, b->data, 1, 0);
  sleep(c, &ide_lock);
  ide_finish(c);
  b->flags |= B_VALID;
  release(&ide_lock);
}

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

