// This a dirt simple boot loader, whose sole job is to boot
// an elf kernel image from the first IDE hard disk.
//
// DISK LAYOUT
//  * This program(boot.S and main.c) is the bootloader.  It should
//    be stored in the first sector of the disk.
//
//  * The 2nd sector onward holds the kernel image.
//
//  * The kernel image must be in ELF format.
//
// BOOT UP STEPS
//  * when the CPU boots it loads the BIOS into memory and executes it
//
//  * the BIOS intializes devices, sets of the interrupt routines, and
//    reads the first sector of the boot device(e.g., hard-drive)
//    into memory and jumps to it.
//
//  * Assuming this boot loader is stored in the first sector of the
//    hard-drive, this code takes over...
//
//  * control starts in bootloader.S -- which sets up protected mode,
//    and a stack so C code then run, then calls cmain()
//
//  * cmain() in this file takes over, 
//    reads in the kernel and jumps to it.

#include "types.h"
#include "elf.h"
#include "x86.h"

#define SECTSIZE  512
#define ELFHDR    ((struct elfhdr*) 0x10000) // scratch space

void readsect(void*, uint);
void readseg(uint, uint, uint);

void
cmain(void)
{
  struct proghdr *ph, *eph;

  // read 1st page off disk
  readseg((uint) ELFHDR, SECTSIZE*8, 0);

  // is this a valid ELF?
  if(ELFHDR->magic != ELF_MAGIC)
    goto bad;

  // load each program segment (ignores ph flags)
  ph = (struct proghdr*) ((uchar*) ELFHDR + ELFHDR->phoff);
  eph = ph + ELFHDR->phnum;
  for(; ph < eph; ph++)
    readseg(ph->va, ph->memsz, ph->offset);

  // call the entry point from the ELF header
  // note: does not return!
  ((void(*)(void)) (ELFHDR->entry & 0xFFFFFF))();

bad:
  outw(0x8A00, 0x8A00);
  outw(0x8A00, 0x8E00);
  for(;;)
    ;
}

// Read 'count' bytes at 'offset' from kernel into virtual address 'va'.
// Might copy more than asked
void
readseg(uint va, uint count, uint offset)
{
  uint end_va;

  va &= 0xFFFFFF;
  end_va = va + count;

  // round down to sector boundary
  va &= ~(SECTSIZE - 1);

  // translate from bytes to sectors, and kernel starts at sector 1
  offset = (offset / SECTSIZE) + 1;

  // If this is too slow, we could read lots of sectors at a time.
  // We'd write more to memory than asked, but it doesn't matter --
  // we load in increasing order.
  while(va < end_va) {
    readsect((uchar*) va, offset);
    va += SECTSIZE;
    offset++;
  }
}

void
waitdisk(void)
{
  // wait for disk reaady
  while((inb(0x1F7) & 0xC0) != 0x40)
    ;
}

void
readsect(void *dst, uint offset)
{
  // wait for disk to be ready
  waitdisk();

  outb(0x1F2, 1);   // count = 1
  outb(0x1F3, offset);
  outb(0x1F4, offset >> 8);
  outb(0x1F5, offset >> 16);
  outb(0x1F6, (offset >> 24) | 0xE0);
  outb(0x1F7, 0x20);  // cmd 0x20 - read sectors

  // wait for disk to be ready
  waitdisk();

  // read a sector
  insl(0x1F0, dst, SECTSIZE/4);
}

