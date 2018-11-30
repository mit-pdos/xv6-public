#include "asm.h"

# Each non-boot CPU ("AP") is started up in response to a STARTUP
# IPI from the boot CPU.  Section B.4.2 of the Multi-Processor
# Specification says that the AP will start in real mode with CS:IP
# set to XY00:0000, where XY is an 8-bit value sent with the
# STARTUP. Thus this code must start at a 4096-byte boundary.
#
# Because this code sets DS to zero, it must sit
# at an address in the low 2^16 bytes.
#
# Bootothers (in main.c) sends the STARTUPs one at a time.
# It copies this code (start) at 0x7000.
# It puts the address of a newly allocated per-core stack in start-4,
# and the address of the place to jump to (mpmain) in start-8.
#
# This code is identical to bootasm.S except:
#   - it does not need to enable A20
#   - it uses the address at start-4 for the %esp
#   - it jumps to the address at start-8 instead of calling bootmain

#define SEG_KCODE 1
#define SEG_KDATA 2

#define CR0_PE    1

.code16           
.globl start
start:
  cli            

  xorw    %ax,%ax
  movw    %ax,%ds
  movw    %ax,%es
  movw    %ax,%ss

  lgdt    gdtdesc
  movl    %cr0, %eax
  orl     $CR0_PE, %eax
  movl    %eax, %cr0

  ljmp    $(SEG_KCODE<<3), $start32

.code32
start32:
  movw    $(SEG_KDATA<<3), %ax
  movw    %ax, %ds
  movw    %ax, %es
  movw    %ax, %ss
  movw    $0, %ax
  movw    %ax, %fs
  movw    %ax, %gs

  # switch to the stack allocated by bootothers()
  movl    start-4, %esp

  # call mpmain()
  call	*(start-8)

  movw    $0x8a00, %ax
  movw    %ax, %dx
  outw    %ax, %dx
  movw    $0x8ae0, %ax
  outw    %ax, %dx
spin:
  jmp     spin

.p2align 2
gdt:
  SEG_NULLASM
  SEG_ASM(STA_X|STA_R, 0x0, 0xffffffff)
  SEG_ASM(STA_W, 0x0, 0xffffffff)

gdtdesc:
  .word   (gdtdesc - gdt - 1)
  .long   gdt
