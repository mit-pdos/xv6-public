	# qemu -kernel starts at 0x1000. the instructions
        # there seem to be provided by qemu, as if it
        # were a ROM. the code at 0x1000 jumps to
        # 0x8000000, the _start function here,
        # in machine mode.
.section .data
.globl stack0
.section .text
.globl mstart
.section .text
.globl _entry
_entry:
	# set up a stack for C.
        # stack0 is declared in start,
        # with 4096 bytes per CPU.
        la sp, stack0
        li a0, 1024*4
	csrr a1, mhartid
        addi a1, a1, 1
        mul a0, a0, a1
        add sp, sp, a0
	# jump to mstart() in start.c
        call mstart
junk:
        j junk
