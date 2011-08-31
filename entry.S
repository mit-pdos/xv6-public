# Multiboot header, for multiboot boot loaders like GNU Grub.
# http://www.gnu.org/software/grub/manual/multiboot/multiboot.html
#
# Using GRUB 2, you can boot xv6 from a file stored in a
# Linux file system by copying kernel or kernelmemfs to /boot
# and then adding this menu entry:
#
# menuentry "xv6" {
# 	insmod ext2
# 	set root='(hd0,msdos1)'
# 	set kernel='/boot/kernel'
# 	echo "Loading ${kernel}..."
# 	multiboot ${kernel} ${kernel}
# 	boot
# }

#include "asm.h"
#include "memlayout.h"
#include "mmu.h"

#define STACK 4096

# Multiboot header.  Data to direct multiboot loader.
.p2align 2
.text
.globl multiboot_header
multiboot_header:
  #define magic 0x1badb002
  #define flags (1<<16 | 1<<0)
  .long magic
  .long flags
  .long (-magic-flags)
  .long multiboot_header  # beginning of image
  .long multiboot_header
  .long edata
  .long end
  .long entry

# Entering xv6 on boot processor.  Machine is mostly set up.
.globl entry
entry:
  # Turn on page size extension for 4Mbyte pages
  movl    %cr4, %eax
  orl     $(CR4_PSE), %eax
  movl    %eax, %cr4
  # Set page directory
  movl    $(V2P_WO(entrypgdir)), %eax
  movl    %eax, %cr3
  # Turn on paging.
  movl    %cr0, %eax
  orl     $(CR0_PG|CR0_WP), %eax
  movl    %eax, %cr0

  # now switch to using addresses above KERNBASE
  # call addresses are pc-relative so we jump though this hoop:
  mov     $relocated, %eax
  jmp     *%eax
relocated:
  # Set up the stack pointer and call into C.
  movl $(stack + STACK), %esp
  call main
spin:
  jmp spin

.comm stack, STACK
