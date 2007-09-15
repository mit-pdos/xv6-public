// Boot loader.
// 
// Part of the boot sector, along with bootasm.S, which calls bootmain().
// bootasm.S has put the processor into protected 32-bit mode.
// bootmain() loads an ELF kernel image from the disk starting at
// sector 1 and then jumps to the kernel entry routine.

#include "types.h"
#include "elf.h"
#include "x86.h"

#define SECTSIZE  512

void readseg(uint, uint, uint);

void
bootmain(void)
{
  struct elfhdr *elf;
  struct proghdr *ph, *eph;
  void (*entry)(void);

  elf = (struct elfhdr*)0x10000;  // scratch space

  // Read 1st page off disk
  readseg((uint)elf, SECTSIZE*8, 0);

  // Is this an ELF executable?
  if(elf->magic != ELF_MAGIC)
    goto bad;

  // Load each program segment (ignores ph flags).
  ph = (struct proghdr*)((uchar*)elf + elf->phoff);
  eph = ph + elf->phnum;
  for(; ph < eph; ph++)
    readseg(ph->va & 0xFFFFFF, ph->memsz, ph->offset);

  // Call the entry point from the ELF header.
  // Does not return!
  entry = (void(*)(void))(elf->entry & 0xFFFFFF);
  entry();

bad:
  outw(0x8A00, 0x8A00);
  outw(0x8A00, 0x8E00);
  for(;;)
    ;
}

void
waitdisk(void)
{
  // Wait for disk ready.
  while((inb(0x1F7) & 0xC0) != 0x40)
    ;
}

// Read a single sector at offset into dst.
void
readsect(void *dst, uint offset)
{
  // Issue command.
  waitdisk();
  outb(0x1F2, 1);   // count = 1
  outb(0x1F3, offset);
  outb(0x1F4, offset >> 8);
  outb(0x1F5, offset >> 16);
  outb(0x1F6, (offset >> 24) | 0xE0);
  outb(0x1F7, 0x20);  // cmd 0x20 - read sectors

  // Read data.
  waitdisk();
  insl(0x1F0, dst, SECTSIZE/4);
}

// Read 'count' bytes at 'offset' from kernel into virtual address 'va'.
// Might copy more than asked.
void
readseg(uint va, uint count, uint offset)
{
  uint eva;

  eva = va + count;

  // Round down to sector boundary.
  va &= ~(SECTSIZE - 1);

  // Translate from bytes to sectors; kernel starts at sector 1.
  offset = (offset / SECTSIZE) + 1;

  // If this is too slow, we could read lots of sectors at a time.
  // We'd write more to memory than asked, but it doesn't matter --
  // we load in increasing order.
  for(; va < eva; va += SECTSIZE, offset++)
    readsect((uchar*)va, offset);
}
