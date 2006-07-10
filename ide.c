/*
 * Minimal PIO-based (non-interrupt-driven) IDE driver code.
 * For information about what all this IDE/ATA magic means,
 * see the materials available on the class references page.
 */

#include "types.h"
#include "param.h"
#include "mmu.h"
#include "proc.h"
#include "defs.h"
#include "x86.h"

#define IDE_BSY		0x80
#define IDE_DRDY	0x40
#define IDE_DF		0x20
#define IDE_ERR		0x01

static int diskno = 0;
int disk_channel;

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
  cprintf("ide_init: enable IRQ 14\n");
  irq_setmask_8259A(irq_mask_8259A & ~(1<<14));
  ide_wait_ready(0);
}

void
ide_intr(void)
{
  cprintf("ide_intr\n");
  wakeup(&disk_channel);
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

  cprintf("Device 1 presence: %d\n", (x < 1000));
  return (x < 1000);
}

void
ide_set_disk(int d)
{
  if (d != 0 && d != 1)
    panic("bad disk number");
  diskno = d;
}

int
ide_start_read(uint32_t secno, void *dst, unsigned nsecs)
{
  if(nsecs > 256)
    panic("ide_start_read: nsecs too large");

  ide_wait_ready(0);

  outb(0x3f6, 0);
  outb(0x1F2, nsecs);
  outb(0x1F3, secno & 0xFF);
  outb(0x1F4, (secno >> 8) & 0xFF);
  outb(0x1F5, (secno >> 16) & 0xFF);
  outb(0x1F6, 0xE0 | ((diskno&1)<<4) | ((secno>>24)&0x0F));
  outb(0x1F7, 0x20);	// CMD 0x20 means read sector

  return 0;
}

int
ide_read(uint32_t secno, void *dst, unsigned nsecs)
{
  int r;

  for (; nsecs > 0; nsecs--, dst += 512) {
    if ((r = ide_wait_ready(1)) < 0)
      return r;
    insl(0x1F0, dst, 512/4);
  }
  return 0;
}

int
ide_write(uint32_t secno, const void *src, unsigned nsecs)
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
  outb(0x1F7, 0x30);	// CMD 0x30 means write sector

  for (; nsecs > 0; nsecs--, src += 512) {
    if ((r = ide_wait_ready(1)) < 0)
      return r;
    outsl(0x1F0, src, 512/4);
  }

  return 0;
}
