OUTPUT_FORMAT("elf64-x86-64", "elf64-x86-64", "elf64-x86-64")
OUTPUT_ARCH(i386:x86-64)
ENTRY(xxx)

SECTIONS
{
	. = 0xFFFFFFFFC0100000;
	PROVIDE(text = .);
	.text : AT(0x100000) {
		*(.text .stub .text.* .gnu.linkonce.t.*)
	}
	.rodata : {
		*(.rodata .rodata.* .gnu.linkonce.r.*)
	}
	. = ALIGN(0x1000);
	PROVIDE(sprof = .);
	.prof : {
		*(.prof)
	}
	PROVIDE(eprof = .);
	. = ALIGN(0x1000);
	.data : {
		*(.data .data.* .gnu.linkonce.d.*)
                *(.got) *(.igot)
                *(.got.plt)  *(.igot.plt)
                *(.eh_frame_hdr)
                *(.eh_frame)
                *(.gcc_except_table .gcc_except_table.*)
	}
	PROVIDE(edata = .);
	.bss : {
		*(.bss .bss.* .gnu.linkonce.b.*)
	}
        /* 2MByte align, because we set NX on 2MByte super pages. */
	. = ALIGN(0x200000);
	PROVIDE(end = .);
}
