/*
 * Simple PIO-based (non-DMA) IDE driver code.
 */

#include "types.h"
#include "param.h"
#include "mmu.h"
#include "proc.h"
#include "defs.h"
#include "x86.h"
#include "traps.h"
#include "spinlock.h"

#define IDE_BSY     0x80
#define IDE_DRDY    0x40
#define IDE_DF		0x20
#define IDE_ERR		0x01

struct ide_request {
  int diskno;
  uint secno;
  void *addr;
  uint nsecs;
  uint read;
};

struct ide_request request[NREQUEST];
int head, tail;
struct spinlock ide_lock;
int disk_1_present;
int disk_channel;

int ide_probe_disk1(void);

static int
ide_wait_ready(int check_error)
{
  int r;

  while (((r = inb(0x1F7)) & (IDE_BSY|IDE_DRDY)) != IDE_DRDY)
    /* do nothing */;

  if (check_error && (r & (IDE_DF|IDE_ERR)) != 0)
    return -1;
  return 0;
}

void
ide_init(void)
{
  initlock(&ide_lock, "ide");
  ioapic_enable (IRQ_IDE, ncpu - 1);
  ide_wait_ready(0);
  disk_1_present = ide_probe_disk1();
}

void
ide_intr(void)
{
  acquire(&ide_lock);
  wakeup(&request[tail]);
  release(&ide_lock);
}

int
ide_probe_disk1(void)
{
  int r, x;

  // wait for Device 0 to be ready
  ide_wait_ready(0);

  // switch to Device 1
  outb(0x1F6, 0xE0 | (1<<4));

  // check for Device 1 to be ready for a while
  for (x = 0; x < 1000 && (r = inb(0x1F7)) == 0; x++)
    /* do nothing */;

  // switch back to Device 0
  outb(0x1F6, 0xE0 | (0<<4));

  return (x < 1000);
}

void
ide_start_request (void)
{
  struct ide_request *r;

  if (head != tail) {
    r = &request[tail];
    ide_wait_ready(0);
    outb(0x3f6, 0);  // generate interrupt
    outb(0x1F2, r->nsecs);
    outb(0x1F3, r->secno & 0xFF);
    outb(0x1F4, (r->secno >> 8) & 0xFF);
    outb(0x1F5, (r->secno >> 16) & 0xFF);
    outb(0x1F6, 0xE0 | ((r->diskno&1)<<4) | ((r->secno>>24)&0x0F));
    if (r->read) outb(0x1F7, 0x20); // read
    else {
      outb(0x1F7, 0x30); // write
      outsl(0x1F0, r->addr, 512/4);
    }
  }
}

void *
ide_start_rw(int diskno, uint secno, void *addr, uint nsecs, int read)
{
  struct ide_request *r;

  if(diskno && !disk_1_present)
    panic("ide disk 1 not present");

  while ((head + 1) % NREQUEST == tail)
    sleep (&disk_channel, &ide_lock);

  r = &request[head];
  r->secno = secno;
  r->addr = addr;
  r->nsecs = nsecs;
  r->diskno = diskno;
  r->read = read;

  head = (head + 1) % NREQUEST;

  ide_start_request();

  return r;
}

int
ide_finish(void *c)
{
  int r;
  struct ide_request *req = (struct ide_request *) c;

  if (req->read) {
    if ((r = ide_wait_ready(1)) >= 0)
      insl(0x1F0, req->addr, 512/4);
  }

  if ((head + 1) % NREQUEST == tail) {
    wakeup(&disk_channel);
  }
  
  tail = (tail + 1) % NREQUEST;
  ide_start_request();

  return 0;
}

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

  for (; nsecs > 0; nsecs--, src += 512) {
    if ((r = ide_wait_ready(1)) < 0)
      return r;
    outsl(0x1F0, src, 512/4);
  }

  return 0;
}
