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
	# set up a stack for C; stack0 is declared in start.
        la sp, stack0
        addi sp, sp, 1024
        addi sp, sp, 1024
        addi sp, sp, 1024
        addi sp, sp, 1024
	# jump to mstart() in start.c
        call mstart
junk:
        j junk
