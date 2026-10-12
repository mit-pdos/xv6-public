// Simple PIO-based (non-DMA) IDE driver code.

#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "x86.h"
#include "traps.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "fs.h"
#include "buf.h"

#define SECTOR_SIZE   512
#define IDE_BSY       0x80
#define IDE_DRDY      0x40
#define IDE_DF        0x20
#define IDE_ERR       0x01

#define IDE_CMD_READ  0x20
#define IDE_CMD_WRITE 0x30
#define IDE_CMD_RDMUL 0xc4
#define IDE_CMD_WRMUL 0xc5

// idequeue points to the buf now being read/written to the disk.
// idequeue->qnext points to the next buf to be processed.
// You must hold idelock while manipulating queue.

static struct spinlock idelock;
static struct buf *idequeue;

static int havedisk1;
static int havedisk2;
static void idestart(struct buf*);


/*Abrar : need to add a parameter portno to idewait to recognize
 * which device to wait on
 */
// Wait for IDE disk to become ready.
static int
idewait(int checkerr, int portno)
{
  int r;

  while(((r = inb(portno + 7)) & (IDE_BSY|IDE_DRDY)) != IDE_DRDY)
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
  ioapicenable(IRQ_IDE, ncpu - 1);
  ioapicenable(IRQ_IDE + 1, ncpu - 1);

  // Check if disk 1 is present
  outb(0x1f6, 0xe0 | (1<<4));
  for(i=0; i<1000; i++){
    if(inb(0x1f7) != 0){
      havedisk1 = 1;
      break;
    }
  }

  //Abrar : Check if disk 2 is present
  outb(0x176, 0xe0 | (2<<4));
  for(i=0; i<1000; i++){
    if(inb(0x177) != 0){
      havedisk2 = 1;
      break;
    }
  }

  // Switch back to disk 0.
  outb(0x1f6, 0xe0 | (0<<4));
}

/* Abrar :(credits to website - https://wiki.osdev.org/ATA_PIO_Mode )
 * There is a standardized set of IO ports to control the disks on the buses.
 * The first two buses are called the Primary and Secondary ATA bus,
 * and are almost always controlled by IO ports 0x1F0 through 0x1F7, and 0x170 through 0x177, respectively.
 * The associated Device Control Registers/Alternate Status ports are IO ports 0x3F6, and 0x376, respectively.
 * The standard IRQ for the Primary bus is IRQ14, and IRQ15 for the Secondary bus.
 *
 * Abrar : Therefore use primary bus for device 0 and 1.
 * Use secondary bus for device 2.
 * Since IDE supports only 2 devices per bus, fourth disk can be added to secondary bus.
 * */

// Start the request for b.  Caller must hold idelock.
static void
idestart(struct buf *b)
{
  if(b == 0)
    panic("idestart");
  if(b->dev == 1 && b->blockno >= FSSIZE)
    panic("incorrect blockno");

  int portno;
  if (b->dev <= 1)
    portno = 0x1f0;//primary port
  else
    portno = 0x170;//secondary port

  int sector_per_block =  BSIZE/SECTOR_SIZE;
  int sector = b->blockno * sector_per_block;
  int read_cmd = (sector_per_block == 1) ? IDE_CMD_READ :  IDE_CMD_RDMUL;
  int write_cmd = (sector_per_block == 1) ? IDE_CMD_WRITE : IDE_CMD_WRMUL;

  if (sector_per_block > 7) panic("idestart");

  idewait(0, portno);
  if (b->dev <= 1) {
    outb(0x3f6, 0);  // generate interrupt to device 0 and 1
  } else {
    outb(0x376, 0);  // generate interrupt to device 2
  }

  outb(portno + 2, sector_per_block);//number of sectors
  outb(portno + 3, sector & 0xff);
  outb(portno + 4, (sector >> 8) & 0xff);
  outb(portno + 5, (sector >> 16) & 0xff);
  outb(portno + 6, 0xe0 | ((b->dev&1)<<4) | ((sector>>24)&0x0f));
  if(b->flags & B_DIRTY){
    outb(portno + 7, write_cmd);
    outsl(portno, b->data, BSIZE/4);
  } else {
    outb(portno + 7, read_cmd);
  }

}


/* Abrar : need to add a flag to indicate which device to send the interrupt to.
 * As a result of this we need to change a line of code in trap.c
 */
// Interrupt handler.
void
ideintr(int flag)
{
  struct buf *b;
  int portno;
  // First queued buffer is the active request.
  acquire(&idelock);
  if(flag == 0)//primary bus(device 0 and 1)
	  portno = 0x1f0;
  else // secondary bus(device 2)
	  portno = 0x170;

  if((b = idequeue) == 0){
    release(&idelock);
    return;
  }
  idequeue = b->qnext;

  // Read data if needed.
  if(!(b->flags & B_DIRTY) && idewait(1, portno) >= 0)
    insl(portno, b->data, BSIZE/4);

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

  if(!holdingsleep(&b->lock))
    panic("iderw: buf not locked");
  if((b->flags & (B_VALID|B_DIRTY)) == B_VALID)
    panic("iderw: nothing to do");
  if(b->dev != 0 && !havedisk1)
    panic("iderw: ide disk 1 not present");
  if(b->dev != 0 && !havedisk2)
    panic("iderw: ide disk 2 not present");


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
