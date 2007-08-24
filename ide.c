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

// IDE request queue.
// ide_queue points to the buf now being read/written to the disk.
// ide_queue->qnext points to the next buf to be processed.
// Must hold ide_lock while manipulating queue.

static struct spinlock ide_lock;
static struct buf *ide_queue;

static int disk_1_present;

static int ide_probe_disk1(void);
static void ide_start_request();

//PAGEBREAK: 10
// Wait for IDE disk to become ready.
static int
ide_wait_ready(int check_error)
{
  int r;

  while(((r = inb(0x1F7)) & (IDE_BSY|IDE_DRDY)) != IDE_DRDY)
    ;

  if(check_error && (r & (IDE_DF|IDE_ERR)) != 0)
    return -1;
  return 0;
}

void
ide_init(void)
{
  initlock(&ide_lock, "ide");
  irq_enable(IRQ_IDE);
  ioapic_enable(IRQ_IDE, ncpu - 1);
  ide_wait_ready(0);
  disk_1_present = ide_probe_disk1();
}

// Probe to see if disk 1 exists (we assume disk 0 exists).
static int
ide_probe_disk1(void)
{
  int r, x;

  // wait for Device 0 to be ready
  ide_wait_ready(0);

  // switch to Device 1
  outb(0x1F6, 0xE0 | (1<<4));

  // check for Device 1 to be ready for a while
  for(x = 0; x < 1000 && (r = inb(0x1F7)) == 0; x++)
    ;

  // switch back to Device 0
  outb(0x1F6, 0xE0 | (0<<4));

  return x < 1000;
}

// Interrupt handler - wake up the request that just finished.
void
ide_intr(void)
{
  acquire(&ide_lock);
  if(ide_queue){
    //cprintf("intr %x\n", ide_queue);
    if((ide_queue->flags & B_WRITE) == 0)
      if(ide_wait_ready(1) >= 0)
        insl(0x1F0, ide_queue->data, 512/4);
    ide_queue->done = 1;
    wakeup(ide_queue);
    ide_queue = ide_queue->qnext;
    ide_start_request();
  } else {
    cprintf("stray ide interrupt\n");
  }
  release(&ide_lock);
}

// Start the next request in the queue.
// Caller must hold ide_lock.
static void
ide_start_request (void)
{
  if(ide_queue){
    ide_wait_ready(0);
    //cprintf("start %x\n", ide_queue);
    outb(0x3f6, 0);  // generate interrupt
    outb(0x1F2, 1);  // number of sectors
    outb(0x1F3, ide_queue->sector & 0xFF);
    outb(0x1F4, (ide_queue->sector >> 8) & 0xFF);
    outb(0x1F5, (ide_queue->sector >> 16) & 0xFF);
    outb(0x1F6, 0xE0 |
         ((ide_queue->dev & 1)<<4) |
         ((ide_queue->sector>>24)&0x0F));
    if(ide_queue->flags & B_WRITE){
      outb(0x1F7, IDE_CMD_WRITE);
      outsl(0x1F0, ide_queue->data, 512/4);
    } else {
      outb(0x1F7, IDE_CMD_READ);
    }
  }
}

//PAGEBREAK: 30
// Queue up a disk operation and wait for it to finish.
// b must have B_BUSY set.
void
ide_rw(struct buf *b)
{
  struct buf **pp;

  if((b->dev & 0xff) && !disk_1_present)
    panic("ide disk 1 not present");

  acquire(&ide_lock);

  b->done = 0;
  b->qnext = 0;

  // cprintf("enqueue %x %x\n", b, ide_queue);

  // append b to ide_queue
  pp = &ide_queue;
  while(*pp)
    pp = &(*pp)->qnext;
  *pp = b;
  
  if(ide_queue == b)
    ide_start_request();
  
  while(!b->done)
    sleep(b, &ide_lock);
  
  release(&ide_lock);
}
