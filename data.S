// The kernel layout is:
//
//     text
//     rodata
//     data
//     bss
//
// Conventionally, Unix linkers provide pseudo-symbols
// etext, edata, and end, at the end of the text, data, and bss.
// For the kernel mapping, we need the address at the beginning
// of the data section, but that's not one of the conventional
// symbols, because the convention started before there was a
// read-only rodata section between text and data.
//
// To get the address of the data section, we define a symbol
// named data and make sure this is the first object passed to
// the linker, so that it will be the first symbol in the data section.
//
// Alternative approaches would be to parse our own ELF header
// or to write a linker script, but this is simplest.

.data
.globl data
.align 4096
data:
  .word 1

