#include "asm.h"

# Start an Application Processor. This must be placed on a 4KB boundary
# somewhere in the 1st MB of conventional memory (APBOOTSTRAP). However,
# due to some shortcuts below it's restricted further to within the 1st
# 64KB. The AP starts in real-mode, with
#   CS selector set to the startup memory address/16;
#   CS base set to startup memory address;
#   CS limit set to 64KB;
#   CPL and IP set to 0.
#
# mp.c causes each non-boot CPU in turn to jump to start.
# mp.c puts the correct %esp in start-4, and the place to jump
# to in start-8.

.set PROT_MODE_CSEG,0x8         # code segment selector
.set PROT_MODE_DSEG,0x10        # data segment selector
.set CR0_PE_ON,0x1              # protected mode enable flag

.globl start
start:
  .code16                     # This runs in real mode
  cli                         # Disable interrupts
  cld                         # String operations increment

  # Set up the important data segment registers (DS, ES, SS).
  xorw    %ax,%ax             # Segment number zero
  movw    %ax,%ds             # -> Data Segment
  movw    %ax,%es             # -> Extra Segment
  movw    %ax,%ss             # -> Stack Segment

  # Set up the stack pointer, growing downward from 0x7000-8.
  movw    $start-8,%sp        # Stack Pointer

  # Switch from real to protected mode
  #  The descriptors in our GDT allow all physical memory to be accessed.
  #  Furthermore, the descriptors have base addresses of 0, so that the
  #  segment translation is a NOP, ie. virtual addresses are identical to
  #  their physical addresses.  With this setup, immediately after
  #  enabling protected mode it will still appear to this code
  #  that it is running directly on physical memory with no translation.
  #  This initial NOP-translation setup is required by the processor
  #  to ensure that the transition to protected mode occurs smoothly.

  lgdt    gdtdesc             # load GDT -- mandatory in protected mode
  movl    %cr0, %eax          # turn on protected mode
  orl     $CR0_PE_ON, %eax    #
  movl    %eax, %cr0          #

  # CPU magic: jump to relocation, flush prefetch queue, and reload %cs
  # Has the effect of just jmp to the next instruction, but simultaneous
  # loads CS with $PROT_MODE_CSEG.
  ljmp    $PROT_MODE_CSEG, $protcseg

# We are now in 32-bit protected mode (hence the .code32)
.code32
protcseg:
  # Set up the protected-mode data segment registers
  movw    $PROT_MODE_DSEG, %ax    # Our data segment selector
  movw    %ax, %ds                # -> DS: Data Segment
  movw    %ax, %es                # -> ES: Extra Segment
  movw    %ax, %fs                # -> FS
  movw    %ax, %gs                # -> GS
  movw    %ax, %ss                # -> SS: Stack Segment

  movl    start-8, %eax
  movl    start-4, %esp
  jmp     *%eax

.p2align 2                                # force 4 byte alignment
gdt:
  SEG_NULLASM                             # null seg
  SEG_ASM(STA_X|STA_R, 0x0, 0xffffffff)   # code seg
  SEG_ASM(STA_W, 0x0, 0xffffffff)         # data seg

gdtdesc:
  .word   0x17                            # sizeof(gdt) - 1
  .long   gdt                             # address gdt
