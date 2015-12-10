// Boot loader.
//
// Part of the boot block, along with bootasm.S, which calls bootmain().
// bootasm.S has put the processor into protected 32-bit mode.
// bootmain() loads an ELF kernel image from the disk starting at
// sector 1 and then jumps to the kernel entry routine.

#include "types.h"
#include "elf.h"
#include "x86.h"
#include "ide.h"

void readseg(uchar*, uint, uint);

void
bootmain(void)
{
  struct elfhdr *elf;
  struct proghdr *ph, *eph;
  void (*entry)(void);
  uchar* pa;

  elf = (struct elfhdr*)0x10000;  // scratch space

  // Read 1st page off disk
  readseg((uchar*)elf, 4096, 0);

  // Is this an ELF executable?
  if(elf->magic != ELF_MAGIC)
    return;  // let bootasm.S handle error

  // Load each program segment (ignores ph flags).
  ph = (struct proghdr*)((uchar*)elf + elf->phoff);
  eph = ph + elf->phnum;
  for(; ph < eph; ph++){
    pa = (uchar*)ph->paddr;
    readseg(pa, ph->filesz, ph->off);
    if(ph->memsz > ph->filesz)
      stosb(pa + ph->filesz, 0, ph->memsz - ph->filesz);
  }

  // Call the entry point from the ELF header.
  // Does not return!
  entry = (void(*)(void))(elf->entry);
  entry();
}

static int
diskready(void)
{
  uchar status = inb(IDE_DATA_PRIMARY+IDE_REG_STATUS);
  return (status & (IDE_BSY|IDE_DRDY)) == IDE_DRDY;
}

void
waitdisk(void)
{
  // Wait for disk ready.
  while(!diskready())
    ;
}

// Read a single sector at offset into dst.
void
readsect(void *dst, uint offset)
{
  // Issue command.
  waitdisk();
  outb(IDE_DATA_PRIMARY+IDE_REG_SECTORS, 1);
  outb(IDE_DATA_PRIMARY+IDE_REG_LBA0, offset & 0xff);
  outb(IDE_DATA_PRIMARY+IDE_REG_LBA1, (offset >> 8) & 0xff);
  outb(IDE_DATA_PRIMARY+IDE_REG_LBA2, (offset >> 16) & 0xff);
  outb(IDE_DATA_PRIMARY+IDE_REG_DISK,
    ((offset >> 24) & 0x0f) | IDE_DISK_LBA);
  outb(IDE_DATA_PRIMARY+IDE_REG_COMMAND, IDE_CMD_READ);

  // Read data.
  waitdisk();
  insl(IDE_DATA_PRIMARY+IDE_REG_DATA, dst, SECTOR_SIZE/4);
}

// Read 'count' bytes at 'offset' from kernel into physical address 'pa'.
// Might copy more than asked.
void
readseg(uchar* pa, uint count, uint offset)
{
  uchar* epa;

  epa = pa + count;

  // Round down to sector boundary.
  pa -= offset % SECTOR_SIZE;

  // Translate from bytes to sectors; kernel starts at sector 1.
  offset = (offset / SECTOR_SIZE) + 1;

  // If this is too slow, we could read lots of sectors at a time.
  // We'd write more to memory than asked, but it doesn't matter --
  // we load in increasing order.
  for(; pa < epa; pa += SECTOR_SIZE, offset++)
    readsect(pa, offset);
}
