// Simple PIO-based (non-DMA) IDE driver code.

#include "types.h"
#include "param.h"
#include "mmu.h"
#include "proc.h"
#include "defs.h"
#include "x86.h"
#include "traps.h"
#include "spinlock.h"
#include "buf.h"

#define IDE_BSY       0x80
#define IDE_DRDY      0x40
#define IDE_DF        0x20
#define IDE_ERR       0x01

#define IDE_CMD_READ  0x20
#define IDE_CMD_WRITE 0x30

// ide_queue points to the buf now being read/written to the disk.
// ide_queue->qnext points to the next buf to be processed.
// You must hold ide_lock while manipulating queue.

static struct spinlock ide_lock;
static struct buf *ide_queue;

static int disk_1_present;
static void ide_start_request();

// Wait for IDE disk to become ready.
static int
ide_wait_ready(int check_error)
{
  int r;

  while(((r = inb(0x1f7)) & IDE_BSY) || !(r & IDE_DRDY))
    ;
  if(check_error && (r & (IDE_DF|IDE_ERR)) != 0)
    return -1;
  return 0;
}

void
ide_init(void)
{
  int i;

  initlock(&ide_lock, "ide");
  irq_enable(IRQ_IDE);
  ioapic_enable(IRQ_IDE, ncpu - 1);
  ide_wait_ready(0);
  
  // Check if disk 1 is present
  outb(0x1f6, 0xE0 | (1<<4));
  for(i=0; i<1000; i++){
    if(inb(0x1f7) != 0){
      disk_1_present = 1;
      break;
    }
  }
  
  // Switch back to disk 0.
  outb(0x1f6, 0xE0 | (0<<4));
}

// Start the request for b.  Caller must hold ide_lock.
static void
ide_start_request(struct buf *b)
{
  if(b == 0)
    panic("ide_start_request");

  ide_wait_ready(0);
  outb(0x3f6, 0);  // generate interrupt
  outb(0x1f2, 1);  // number of sectors
  outb(0x1f3, b->sector & 0xff);
  outb(0x1f4, (b->sector >> 8) & 0xff);
  outb(0x1f5, (b->sector >> 16) & 0xff);
  outb(0x1f6, 0xE0 | ((b->dev&1)<<4) | ((b->sector>>24)&0x0f));
  if(b->flags & B_WRITE){
    outb(0x1f7, IDE_CMD_WRITE);
    outsl(0x1f0, b->data, 512/4);
  }else{
    outb(0x1f7, IDE_CMD_READ);
  }
}

// Interrupt handler.
void
ide_intr(void)
{
  struct buf *b;

  acquire(&ide_lock);
  if((b = ide_queue) == 0){
    cprintf("stray ide interrupt\n");
    release(&ide_lock);
    return;
  }

  // Read data if needed.
  if((b->flags & B_WRITE) == 0 && ide_wait_ready(1) >= 0)
    insl(0x1f0, b->data, 512/4);
  
  // Wake process waiting for this buf.
  b->done = 1;
  wakeup(b);
  
  // Start disk on next buf in queue.
  if((ide_queue = b->qnext) != 0)
    ide_start_request(ide_queue);

  release(&ide_lock);
}

//PAGEBREAK!
// Queue a disk operation and wait for it to finish.
void
ide_rw(struct buf *b)
{
  struct buf **pp;

  if(!(b->flags & B_BUSY))
    panic("ide_rw: buf not busy");
  if(b->dev != 0 && !disk_1_present)
    panic("ide disk 1 not present");

  acquire(&ide_lock);

  // Append b to ide_queue.
  b->done = 0;
  b->qnext = 0;
  for(pp=&ide_queue; *pp; pp=&(*pp)->qnext)
    ;
  *pp = b;
  
  // Start disk if necessary.
  if(ide_queue == b)
    ide_start_request(b);
  
  // Wait for request to finish.
  while(!b->done)
    sleep(b, &ide_lock);
  release(&ide_lock);
}
