OUTPUT_FORMAT("elf64-x86-64", "elf64-x86-64", "elf64-x86-64")
OUTPUT_ARCH(i386:x86-64)

SECTIONS
{
	. = 0xFFFFFF0000100000;
	PROVIDE(text = .);
	.text : AT(0x100000) {
		*(.text .stub .text.* .gnu.linkonce.t.*)
	}
	.rodata : {
		*(.rodata .rodata.* .gnu.linkonce.r.*)
	}

	/* Include debugging information in kernel memory */
	.stab : {
		PROVIDE(__STAB_BEGIN__ = .);
		*(.stab);
		PROVIDE(__STAB_END__ = .);
		BYTE(0)		/* Force the linker to allocate space
				   for this section */
	}

	.stabstr : {
		PROVIDE(__STABSTR_BEGIN__ = .);
		*(.stabstr);
		PROVIDE(__STABSTR_END__ = .);
		BYTE(0)		/* Force the linker to allocate space
				   for this section */
	}

	. = ALIGN(0x1000);

        /* Conventionally, Unix linkers provide pseudo-symbols
         * etext, edata, and end, at the end of the text, data, and bss.
         * For the kernel mapping, we need the address at the beginning
         * of the data section, but that's not one of the conventional
         * symbols, because the convention started before there was a
         * read-only rodata section between text and data. */
        PROVIDE(data = .);
	.data : {
		*(.data)
	}
	bss : {
		PROVIDE(edata = .);
		*(.bss)
		*(COMMON)
		PROVIDE(end = .);
	}
}
