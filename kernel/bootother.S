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

#define SEG_ASM(type,base,lim)                                  \
        .word (((lim) >> 12) & 0xffff), ((base) & 0xffff);      \
        .byte (((base) >> 16) & 0xff), (0x90 | (type)),         \
                (0xC0 | (((lim) >> 28) & 0xf)), (((base) >> 24) & 0xff)
        
#define PROT_MODE_CSEG 0x8
#define PROT_MODE_DSEG 0x10
#define CR0_PE_ON      0x1
        
.code16           
.globl start
start:
	cli
	wbinvd
	mov $0, %ax
	mov %ax, %ds
	cld

	# Announce we made it this far.
	# Useful for debugging in bootothers.
	movl $0x0a55face, start - 64
        
	lgdt gdtdesc
	movl %cr0, %eax
	orl $CR0_PE_ON, %eax
	movl %eax, %cr0
        # Jump to next instruction, but in 32-bit code segment.
        # Switches processor into 32-bit mode.
	ljmp $PROT_MODE_CSEG, $protcseg

.code32
protcseg:
	movw $PROT_MODE_DSEG, %ax
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %fs
	movw %ax, %gs
	movw %ax, %ss

        movl $start-12, %esp
	movl start-4, %ecx
	jmp *%ecx

.align 4
gdt:
	.quad 0 # null seg
	SEG_ASM(0xa, 0x0, 0xffffffff) # code seg
	SEG_ASM(0x2, 0x0, 0xffffffff) # data seg

.align 16
gdtdesc:
	.word	0x17 # sizeof(gdt) - 1
gdtaddr:
	.long	gdt  # address gdt
