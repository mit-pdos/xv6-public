/* This linker script tells the linker how the memory
 * of xv6 executables should be laid out
 */
OUTPUT_FORMAT("elf32-i386", "elf32-i386", "elf32-i386")
OUTPUT_ARCH(i386)
ENTRY(main)

SECTIONS {
  . = 0x0;
  .text : { *(.text .stub .text.* .gnu.linkonce.t.*) }
  .rodata : { *(.rodata .rodata.* .gnu.linkonce.r.*) }
  .data : { *(.data) }
  .bss : { *(.bss) }
  /DISCARD/ : {
    *(.eh_frame .note.GNU-stack .note.gnu.property)
  }
}
