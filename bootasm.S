#include "asm.h"

.set PROT_MODE_CSEG,0x8         # code segment selector
.set PROT_MODE_DSEG,0x10        # data segment selector
.set CR0_PE_ON,0x1              # protected mode enable flag

#########################################################################
# ENTRY POINT for the bootstrap processor
#   This code should be stored in the first sector of the hard disk.
#   After the BIOS initializes the hardware on startup or system reset,
#   it loads this code at physical address 0x7c00 - 0x7d00 (512 bytes).
#   Then the BIOS jumps to the beginning of it, address 0x7c00,
#   while running in 16-bit real-mode (8086 compatibility mode).
#   The Code Segment register (CS) is initially zero on entry.
#
# This code switches into 32-bit protected mode so that all of
# memory can accessed, then calls into C.
#########################################################################

.globl start                      # Entry point
start:
.code16                           # This runs in real mode
  cli                             # Disable interrupts
  cld                             # String operations increment

  # Set up the important data segment registers (DS, ES, SS).
  xorw    %ax,%ax                 # Segment number zero
  movw    %ax,%ds                 # -> Data Segment
  movw    %ax,%es                 # -> Extra Segment
  movw    %ax,%ss                 # -> Stack Segment

  # Set up the stack pointer, growing downward from 0x7c00.
  movw    $start,%sp              # Stack Pointer

  # Enable A20:
  #   For fascinating historical reasons (related to the fact that
  #   the earliest 8086-based PCs could only address 1MB of physical
  #   memory and subsequent 80286-based PCs wanted to retain maximum
  #   compatibility), physical address line 20 is tied to low when the
  #   machine boots.  Obviously this a bit of a drag for us, especially
  #   when trying to address memory above 1MB.  This code undoes this.

seta20.1:
  inb     $0x64,%al               # Get status
  testb   $0x2,%al                # Busy?
  jnz     seta20.1                # Yes
  movb    $0xd1,%al               # Command: Write
  outb    %al,$0x64               #  output port

seta20.2:
  inb     $0x64,%al               # Get status
  testb   $0x2,%al                # Busy?
  jnz     seta20.2                # Yes
  movb    $0xdf,%al               # Enable
  outb    %al,$0x60               #  A20

# Switch from real to protected mode
#  The descriptors in our GDT allow all physical memory to be accessed.
#  Furthermore, the descriptors have base addresses of 0, so that the
#  segment translation is a NOP, ie. virtual addresses are identical to
#  their physical addresses.  With this setup, immediately after
#  enabling protected mode it will still appear to this code
#  that it is running directly on physical memory with no translation.
#  This initial NOP-translation setup is required by the processor
#  to ensure that the transition to protected mode occurs smoothly.
real_to_prot:
  cli                         # Mandatory since we dont set up an IDT
  lgdt    gdtdesc             # load GDT -- mandatory in protected mode
  movl    %cr0, %eax          # turn on protected mode
  orl     $CR0_PE_ON, %eax    #
  movl    %eax, %cr0          #
  ### CPU magic: jump to relocation, flush prefetch queue, and reload %cs
  ### Has the effect of just jmp to the next instruction, but simultaneous
  ### loads CS with $PROT_MODE_CSEG.
  ljmp    $PROT_MODE_CSEG, $protcseg

#### we are in 32-bit protected mode (hence the .code32)
.code32
protcseg:
  # Set up the protected-mode data segment registers
  movw    $PROT_MODE_DSEG, %ax    # Our data segment selector
  movw    %ax, %ds                # -> DS: Data Segment
  movw    %ax, %es                # -> ES: Extra Segment
  movw    %ax, %fs                # -> FS
  movw    %ax, %gs                # -> GS
  movw    %ax, %ss                # -> SS: Stack Segment
  call cmain                      # finish the boot load from C.
                                  # cmain() should not return
spin:
  jmp spin                        # ..but in case it does, spin

.p2align 2                                # force 4 byte alignment
gdt:
  SEG_NULLASM                             # null seg
  SEG_ASM(STA_X|STA_R, 0x0, 0xffffffff)   # code seg
  SEG_ASM(STA_W, 0x0, 0xffffffff)         # data seg
gdtdesc:
  .word   0x17                            # sizeof(gdt) - 1
  .long   gdt                             # address gdt
