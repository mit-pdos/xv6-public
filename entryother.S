#include "asm.h"
#include "memlayout.h"
#include "mmu.h"
	
# Each non-boot CPU ("AP") is started up in response to a STARTUP
# IPI from the boot CPU.  Section B.4.2 of the Multi-Processor
# Specification says that the AP will start in real mode with CS:IP
# set to XY00:0000, where XY is an 8-bit value sent with the
# STARTUP. Thus this code must start at a 4096-byte boundary.
#
# Because this code sets DS to zero, it must sit
# at an address in the low 2^16 bytes.
#
# Startothers (in main.c) sends the STARTUPs one at a time.
# It copies this code (start) at 0x7000.  It puts the address of
# a newly allocated per-core stack in start-12,the address of the
# place to jump to (apstart32) in start-4, and the physical address
# of entrypgdir in start-12.

.code16           
.globl start
start:
  cli            

  # Zero data segment registers DS, ES, and SS.
  xorw    %ax,%ax
  movw    %ax,%ds
  movw    %ax,%es
  movw    %ax,%ss

  # Switch from real to protected mode.  Use a bootstrap GDT that makes
  # virtual addresses map directly to physical addresses so that the
  # effective memory map doesn't change during the transition.
  lgdt    gdtdesc
  movl    %cr0, %eax
  orl     $CR0_PE, %eax
  movl    %eax, %cr0

  # Complete the transition to 32-bit protected mode by using a long jmp
  # to reload %cs and %eip.  The segment descriptors are set up with no
  # translation, so that the mapping is still the identity mapping.
  ljmpl    $(KCSEG32), $start32

.code32
start32:
 movl $start-12, %esp
 movl start-4, %ecx
 jmp *%ecx

.align 4
gdt:
  SEG_NULLASM
  SEG_ASM(0xa, 0, 0xffffffff)
  SEG_ASM(0x2, 0, 0xffffffff)

.align  16
gdtdesc:
  .word   0x17 # sizeof(gdt)-1
  .long   gdt

