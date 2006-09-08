// Simple PIO-based (non-DMA) IDE driver code.

#include "types.h"
#include "param.h"
#include "mmu.h"
#include "proc.h"
#include "defs.h"
#include "x86.h"
#include "traps.h"
#include "spinlock.h"

#define IDE_BSY       0x80
#define IDE_DRDY      0x40
#define IDE_DF        0x20
#define IDE_ERR       0x01

#define IDE_CMD_READ  0x20
#define IDE_CMD_WRITE 0x30

// IDE request queue.
// The next request will be stored in request[head],
// and the request currently being served by the disk
// is request[tail].
// Must hold ide_lock while manipulating queue.

struct ide_request {
  int diskno;
  uint secno;
  void *addr;
  uint nsecs;
  uint read;
};

static struct ide_request request[NREQUEST];
static int head, tail;
static struct spinlock ide_lock;

static int disk_1_present;
static int disk_queue;

static int ide_probe_disk1(void);

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
  wakeup(&request[tail]);
  release(&ide_lock);
}

// Start the next request in the queue.
static void
ide_start_request (void)
{
  struct ide_request *r;

  if(head != tail) {
    r = &request[tail];
    ide_wait_ready(0);
    outb(0x3f6, 0);  // generate interrupt
    outb(0x1F2, r->nsecs);
    outb(0x1F3, r->secno & 0xFF);
    outb(0x1F4, (r->secno >> 8) & 0xFF);
    outb(0x1F5, (r->secno >> 16) & 0xFF);
    outb(0x1F6, 0xE0 | ((r->diskno&1)<<4) | ((r->secno>>24)&0x0F));
    if(r->read)
      outb(0x1F7, IDE_CMD_READ);
    else {
      outb(0x1F7, IDE_CMD_WRITE);
      outsl(0x1F0, r->addr, 512/4);
    }
  }
}

// Run an entire disk operation.
void
ide_rw(int diskno, uint secno, void *addr, uint nsecs, int read)
{
  struct ide_request *r;

  if(diskno && !disk_1_present)
    panic("ide disk 1 not present");

  acquire(&ide_lock);
  
  // Add request to queue.
  while((head + 1) % NREQUEST == tail)
    sleep(&disk_queue, &ide_lock);

  r = &request[head];
  r->secno = secno;
  r->addr = addr;
  r->nsecs = nsecs;
  r->diskno = diskno;
  r->read = read;
  head = (head + 1) % NREQUEST;

  // Start request if necessary.
  ide_start_request();
  
  // Wait for request to finish.
  sleep(r, &ide_lock);
  
  // Finish request.
  if(read){
    if(ide_wait_ready(1) >= 0)
      insl(0x1F0, addr, 512/4);
  }

  // Remove request from queue.
  if((head + 1) % NREQUEST == tail)
    wakeup(&disk_queue);
  tail = (tail + 1) % NREQUEST;

  // Start next request in queue, if any.
  ide_start_request();

  release(&ide_lock);
}

// Synchronous disk write.
int
ide_write(int diskno, uint secno, const void *src, uint nsecs)
{
  int r;

  if(nsecs > 256)
    panic("ide_write");

  ide_wait_ready(0);

  outb(0x1F2, nsecs);
  outb(0x1F3, secno & 0xFF);
  outb(0x1F4, (secno >> 8) & 0xFF);
  outb(0x1F5, (secno >> 16) & 0xFF);
  outb(0x1F6, 0xE0 | ((diskno&1)<<4) | ((secno>>24)&0x0F));
  outb(0x1F7, 0x30);    // CMD 0x30 means write sector

  for(; nsecs > 0; nsecs--, src += 512) {
    if((r = ide_wait_ready(1)) < 0)
      return r;
    outsl(0x1F0, src, 512/4);
  }

  return 0;
}
