# x86-64 bootstrap, assuming load by MultiBoot-compliant loader.
# The MutliBoot specification is at:
# http://www.gnu.org/software/grub/manual/multiboot/multiboot.html
# GRUB is a MultiBoot loader, as is qemu's -kernel option.

#include "amd64.h"

# KADDR is the kernel virtual address of the first byte of physical memory.
# The linker loads the executable as if starting at KBASE+2MB, but we
# ask the loader to load the kernel at physical 2MB and then set up the
# necessary memory mapping to switch to the higher address.
# The value of KBASE must match the definitions in vmx.h and kernel.ld.
#define KBASE 0xFFFFFFFF80000000 /* -2GB */

# PADDR(x) is the physical memory address corresponding to x.
# Until we set up the memory map, fairly late in this file, we have to
# refer to PADDR(symbol) instead of symbol, so that we use the
# physical address.
#define PADDR(x) ((x) - KBASE)

# DATAMAGIC is a magic value we set in the data segment and then
# look for to make sure that the data segment was loaded in the right place.
#define DATAMAGIC 0x123456789abcdef0

# STACK is the size of the bootstrap stack.
#define STACK 8192

# PANIC prints a message on the screen and stops execution.
#define PANIC(x) \
	movl $PADDR(10f), %edi; \
	call bootpanic; \
	10: .asciz x

# MultiBoot header.
# http://www.gnu.org/software/grub/manual/multiboot/multiboot.html#Header-layout
.align 4
.text
.globl multiboot_header
multiboot_header:
	#define magic 0x1badb002
	#define flags (1<<16 | 1<<0)
	.long magic
	.long flags
	.long (- magic - flags)        # checksum
	.long PADDR(multiboot_header)  # header address
	.long PADDR(multiboot_header)  # load address
	.long PADDR(edata)             # load end address
	.long PADDR(end)               # bss end address
	.long PADDR(start)             # entry address

# Entry point jumped to by boot loader.  Running in 32-bit mode.
# http://www.gnu.org/software/grub/manual/multiboot/multiboot.html#Machine-state
#
#	EAX = 0x2badb002
#	EBX = address of multiboot information structure
#	CS = 32-bit read/execute code segment with identity map
#	DS, ES, FS, GS, SS = 32-bit read/write data segment with identity map
#	A20 gate = enabled
#	CR0 = PE set, PG clear
#	EFLAGS = VM clear, IF clear
#
.code32
.globl start
start:
	# Tell BIOS to do "warm reboot" when we shut down.
	movw $0x1234, 0x472

	# Set up multiboot arguments for cmain.
	movl %eax, %edi
	movl %ebx, %esi

	# Initialize stack.
	movl $PADDR(stack+STACK), %esp

	# Initialize screen.
	call initscreen
	
	# Check for page size extensions.
	movl $1, %eax
	cpuid
	testl $(1<<3), %edx
	jnz 1f
	PANIC("This processor does not support PSE.")
1:

	# Check for 64-bit support.
	movl $0x80000001, %eax
	cpuid
	testl $(1<<29),%edx
	jnz 1f
	PANIC("This processor does not support 64-bit execution.")
1:

	# Zero bss.  QEMU's MultiBoot seems not to.
	# It's possible that the header above is not right, but it looks right.
	# %edi is holding multiboot argument, so save in another register.
	# (The stack is in the bss.)
	movl %edi, %edx
	movl $PADDR(edata), %edi
	movl $PADDR(end), %ecx
	subl $PADDR(edata), %ecx
	movl $0, %eax
	cld
	rep stosb
	movl %edx, %edi

	# Load GDT.
	subl $8, %esp
	movl $PADDR(bootgdt), 4(%esp)
	movw $(8*NUMSEG-1), 2(%esp)
	lgdt 2(%esp)
	addl $8, %esp

	movl $KDSEG, %eax  // data segment selector
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %ss
	movl $0, %eax  // null segment selector
	movw %ax, %fs
	movw %ax, %gs

	# Enter new 32-bit code segment (already in 32-bit mode).
	ljmp $KCSEG32, $PADDR(start32)  // code32 segment selector

start32:
	# Initialize page table.
	call initpagetables

	# Initialize IA-32e mode.  See section 9.8.5 of 253668.pdf.
	
	# Set CR4.PAE and CR4.PSE = 1.
	movl %cr4, %eax
	orl $0x30, %eax
	movl %eax, %cr4

	# Load CR3 with physical base address of level 4 page table.
	movl $PADDR(pml4), %eax
	movl %eax, %cr3
	
	# Enable IA-32e mode by setting IA32_EFER.LME = 1.
	# Also turn on IA32_EFER.SCE (syscall enable).
	movl $0xc0000080, %ecx
	rdmsr
	orl $0x101, %eax
	wrmsr

	# Enable paging by setting CR0.PG = 1.
	movl %cr0, %eax
	orl $0x80000000, %eax	
	movl %eax, %cr0
	nop
	nop

	# Enter 64-bit mode.
	ljmp $KCSEG, $PADDR(start64)  // code64 segment selector

.code64
start64:
	# The linker thinks we are running at start64, but we're actually
	# running at PADDR(start64), so use an explicit calculation to
	# load and jump to the correct address.
	movq $KBASE, %rax
	addq $PADDR(start64hi), %rax
	jmp *%rax

start64hi:
	# Now executing at the real link address.
	# Double-check that data segment is set up correctly.
	movq $DATAMAGIC, %rax
	movq PADDR(datamagic), %rbx
	cmpq %rax, %rbx
	jz 1f
	PANIC("Data segment is not loaded correctly.")
1:
	# Load VA stack pointer
	movabsq $(stack+STACK), %rsp
	# Kill 1GB identity mapping
	movq $0, pml4
	# Clear frame pointer for stack walks, and call into C code.
	movq $0, %rbp
	call cmain
1:      jmp 1b

.code32
.globl bootpanic
bootpanic:
	# This is 32-bit code but can be called from either
	# 32- or 64-bit code, since it only uses instructions
	# in the intersection of both.
	movl $(CGATEXTMEM+80*12*2), %esi  # line 12
1:
	movb (%edi), %bl     # get next char, check for NUL
	testb %bl, %bl
	jz 2f
	movb %bl, (%esi)     # write to screen
	addl $2, %esi        # advance screen pointer
	addl $1, %edi        # advance string pointer
	jmp 1b

2:
	# If we just busy loop here, QEMU will never finish
	# writing our output to the screen.  Instead, keep
	# writing the zero byte over and over.
	movb %bl, (%esi)
	jmp 2b

initscreen:
	pushal

	# Initialize screen.
	# 25 lines of white on black.
	cld
	movl $CGATEXTMEM, %edi
	movl $0x0f20, %eax
	movl $(80*25), %ecx
	rep stosw

	# Hide cursor at beginning of first line.
	movw $0, %di
	call setcursor

	popal
	ret

.globl setcursor
setcursor:
	pushl %edx
	pushl %eax
	pushl %ebx
	movl %edi, %ebx
	movw $0x3d4, %dx
	movw $14, %ax
	outb %al, %dx
	movw $0x3d5, %dx
	movb %bh, %al
	outb %al, %dx
	movw $0x3d4, %dx
	movw $15, %ax
	outb %al, %dx
	movw $0x3d5, %dx
	movb %bl, %al
	outb %al, %dx
	popl %ebx
	popl %eax
	popl %edx
	ret

# Google color bar: spaces with background colors.
colorbar:
	.ascii " \x40 \x40 \x40 \x40 \x40 \x40 \x40 \x40 \x40 \x40"
	.ascii " \x40 \x40 \x40 \x40 \x40 \x40 \x40 \x40 \x40 \x40"
	.ascii " \x10 \x10 \x10 \x10 \x10 \x10 \x10 \x10 \x10 \x10"
	.ascii " \x10 \x10 \x10 \x10 \x10 \x10 \x10 \x10 \x10 \x10"
	.ascii " \x20 \x20 \x20 \x20 \x20 \x20 \x20 \x20 \x20 \x20"
	.ascii " \x20 \x20 \x20 \x20 \x20 \x20 \x20 \x20 \x20 \x20"
	.ascii "\xDB\x0E\xDB\x0E\xDB\x0E\xDB\x0E\xDB\x0E"
	.ascii "\xDB\x0E\xDB\x0E\xDB\x0E\xDB\x0E\xDB\x0E"
	.ascii "\xDB\x0E\xDB\x0E\xDB\x0E\xDB\x0E\xDB\x0E"
	.ascii "\xDB\x0E\xDB\x0E\xDB\x0E\xDB\x0E\xDB\x0E"

# Initial stack
.comm stack, STACK

# Page tables.  See section 4.5 of 253668.pdf.
# We map the first GB of physical memory at 0 and at -2 GB (or 2 GB before
# the end of physical memory).  At boot time we are using the mapping at 0
# but during ordinary execution we use the high mapping.
# The intent is that after bootstrap the kernel can expand this mapping
# to cover all the available physical memory.
# This would be easier if we could use the PS bit to create GB-sized entries
# and skip the pdt table, but not all chips support it, and QEMU doesn't.
.align 4096
.global pml4
pml4:
	.quad PADDR(pdpt0) + (1<<0) + (1<<1)  // present, read/write
	.space 4096 - 16
	.quad PADDR(pdpt1) + (1<<0) + (1<<1)  // present, read/write

.align 4096
pdpt0:
	.quad PADDR(pdt) + (1<<0) + (1<<1)  // present, read/write
	.quad 0
	.space 4096 - 16
       
.align 4096
pdpt1:
	.space 4096 - 16
	.quad PADDR(pdt) + (1<<0) + (1<<1)  // present, read/write
	.quad 0
       
.align 4096
pdt:
	// Filled in below.
	.space 4096

initpagetables:
	pushl %edi
	pushl %ecx
	pushl %eax

	// Set up 64-bit entry in %edx:%eax.
	// Base address 0, present, read/write, large page.
	movl $(0 | 1<<0 | 1<<1 | 1<<7), %eax
	movl $0, %edx

	// Fill in 512 entries at pdt.
	movl $PADDR(pdt), %edi
	movl $512, %ecx
1:
	// Write this 64-bit entry.
	movl %eax, 0(%edi)
	movl %edx, 4(%edi)
	addl $8, %edi
	// 64-bit add to prepare address for next entry.
	// Because this is a large page entry, it covers 512 4k pages (2 MB).
	add $(512*4096), %eax
	adc $0, %edx
	loop 1b

	popl %eax
	popl %ecx
	popl %edi
	ret

# For sanity check above.
.data
datamagic:
	.quad DATAMAGIC
