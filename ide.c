// Simple PIO-based (non-DMA) IDE driver code.

#include "types.h"
#include "defs.h"
#include "param.h"
#include "mmu.h"
#include "proc.h"
#include "x86.h"
#include "traps.h"
#include "spinlock.h"
#include "fs.h"
#include "buf.h"
#include "ide.h"

// idequeue points to the buf now being read/written to the disk.
// idequeue->qnext points to the next buf to be processed.
// You must hold idelock while manipulating queue.

static struct spinlock idelock;
static struct buf *idequeue;

static int havedisk1;
static void idestart(struct buf*);

static int
diskready(int *status)
{
  *status = inb(IDE_DATA_PRIMARY+IDE_REG_STATUS);
  return (*status & (IDE_BSY|IDE_DRDY)) == IDE_DRDY;
}

// Wait for IDE disk to become ready.
static int
idewait(int checkerr)
{
  int r;

  while(!diskready(&r))
    ;
  if(checkerr && (r & (IDE_DF|IDE_ERR)) != 0)
    return -1;
  return 0;
}

void
ideinit(void)
{
  int i;

  initlock(&idelock, "ide");
  picenable(IRQ_IDE);
  ioapicenable(IRQ_IDE, ncpu - 1);
  idewait(0);

  // Check if disk 1 is present
  outb(IDE_DATA_PRIMARY+IDE_REG_DISK, IDE_DISK_LBA | (1<<4));
  for(i=0; i<1000; i++){
    if(inb(IDE_DATA_PRIMARY+IDE_REG_STATUS) != 0){
      havedisk1 = 1;
      break;
    }
  }

  // Switch back to disk 0.
  outb(IDE_DATA_PRIMARY+IDE_REG_DISK, IDE_DISK_LBA | (0<<4));
}

// Start the request for b.  Caller must hold idelock.
static void
idestart(struct buf *b)
{
  if(b == 0)
    panic("idestart");
  if(b->blockno >= FSSIZE)
    panic("incorrect blockno");
  int sector_per_block =  BSIZE/SECTOR_SIZE;
  int sector = b->blockno * sector_per_block;

  if (sector_per_block > 7) panic("idestart");

  idewait(0);
  outb(IDE_CTRL_PRIMARY+IDE_REG_CTRL, 0);  // generate interrupt
  outb(IDE_DATA_PRIMARY+IDE_REG_SECTORS, sector_per_block);
  outb(IDE_DATA_PRIMARY+IDE_REG_LBA0, sector & 0xff);
  outb(IDE_DATA_PRIMARY+IDE_REG_LBA1, (sector >> 8) & 0xff);
  outb(IDE_DATA_PRIMARY+IDE_REG_LBA2, (sector >> 16) & 0xff);
  outb(IDE_DATA_PRIMARY+IDE_REG_DISK,
    IDE_DISK_LBA | ((b->dev&1)<<4) | ((sector>>24)&0x0f));
  if(b->flags & B_DIRTY){
    outb(IDE_DATA_PRIMARY+IDE_REG_COMMAND, IDE_CMD_WRITE);
    outsl(IDE_DATA_PRIMARY+IDE_REG_DATA, b->data, BSIZE/4);
  } else {
    outb(IDE_DATA_PRIMARY+IDE_REG_COMMAND, IDE_CMD_READ);
  }
}

// Interrupt handler.
void
ideintr(void)
{
  struct buf *b;

  // First queued buffer is the active request.
  acquire(&idelock);
  if((b = idequeue) == 0){
    release(&idelock);
    // cprintf("spurious IDE interrupt\n");
    return;
  }
  idequeue = b->qnext;

  // Read data if needed.
  if(!(b->flags & B_DIRTY) && idewait(1) >= 0)
    insl(IDE_DATA_PRIMARY+IDE_REG_DATA, b->data, BSIZE/4);

  // Wake process waiting for this buf.
  b->flags |= B_VALID;
  b->flags &= ~B_DIRTY;
  wakeup(b);

  // Start disk on next buf in queue.
  if(idequeue != 0)
    idestart(idequeue);

  release(&idelock);
}

//PAGEBREAK!
// Sync buf with disk.
// If B_DIRTY is set, write buf to disk, clear B_DIRTY, set B_VALID.
// Else if B_VALID is not set, read buf from disk, set B_VALID.
void
iderw(struct buf *b)
{
  struct buf **pp;

  if(!(b->flags & B_BUSY))
    panic("iderw: buf not busy");
  if((b->flags & (B_VALID|B_DIRTY)) == B_VALID)
    panic("iderw: nothing to do");
  if(b->dev != 0 && !havedisk1)
    panic("iderw: ide disk 1 not present");

  acquire(&idelock);  //DOC:acquire-lock

  // Append b to idequeue.
  b->qnext = 0;
  for(pp=&idequeue; *pp; pp=&(*pp)->qnext)  //DOC:insert-queue
    ;
  *pp = b;

  // Start disk if necessary.
  if(idequeue == b)
    idestart(b);

  // Wait for request to finish.
  while((b->flags & (B_VALID|B_DIRTY)) != B_VALID){
    sleep(b, &idelock);
  }

  release(&idelock);
}
