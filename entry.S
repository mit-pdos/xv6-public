# x86-64 bootstrap, assuming load by MultiBoot-compliant loader.
# The MutliBoot specification is at:
# http://www.gnu.org/software/grub/manual/multiboot/multiboot.html
# GRUB is a MultiBoot loader, as is qemu's -kernel option.

#include "mmu.h"
#include "memlayout.h"  

# STACK is the size of the bootstrap stack.
#define STACK 8192

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
  .long (- magic - flags)  # checksum
  .long V2P_WO(multiboot_header)  # header address
  .long V2P_WO(multiboot_header)  # load address
  .long V2P_WO(edata)       # load end address
  .long V2P_WO(end)         # bss end address
  .long V2P_WO(start)       # entry address

# Entry point jumped to by boot loader.  Running in 32-bit mode.
# http://www.gnu.org/software/grub/manual/multiboot/multiboot.html#Machine-state
#
#       EAX = 0x2badb002
#       EBX = address of multiboot information structure
#       CS = 32-bit read/execute code segment with identity map
#       DS, ES, FS, GS, SS = 32-bit read/write data segment with identity map
#       A20 gate = enabled
#       CR0 = PE set, PG clear
#       EFLAGS = VM clear, IF clear
#
.code32
.globl start
start:
  # Tell BIOS to do "warm reboot" when we shut down.
  movw $0x1234, 0x472

  # Set up multiboot arguments for main.
  movl %eax, %edi
  movl %ebx, %esi

  # Initialize stack.
  movl $V2P_WO(stack+STACK), %esp
  
  # Zero bss.  QEMU's MultiBoot seems not to.
  # It's possible that the header above is not right, but it looks right.
  # %edi is holding multiboot argument, so save in another register.
  # (The stack is in the bss.)
  movl %edi, %edx
  movl $V2P_WO(edata), %edi
  movl $V2P_WO(end), %ecx
  subl $V2P_WO(edata), %ecx
  movl $0, %eax
  cld
  rep stosb
  movl %edx, %edi

  call loadgdt
  
  # Enter new 32-bit code segment (already in 32-bit mode).
  ljmp $KCSEG32, $V2P_WO(start32)  // code32 segment selector
  
start32:
  # Initialize page table.
  call initpagetables
  call init32e
  
  movl $V2P_WO(start64), %eax
  # Enter 64-bit mode.
  ljmp $KCSEG, $V2P_WO(tramp64)  // code64 segment selector

.code64
start64:
  # Load VA of stack
  movabsq $(stack+STACK), %rsp
  # Clear frame pointer for stack walks
  movl $0, %ebp
  # Call into C code.
  call bpmain
  # should not return from bpmain
  jmp .

.code32
.global apstart
apstart:
  call loadgdt
  ljmp $KCSEG32, $V2P_WO(apstart32)  // code32 segment selector
  
apstart32:
  call init32e
  movl $V2P_WO(apstart64), %eax
  ljmp $KCSEG, $V2P_WO(tramp64)  // code64 segment selector

.code64       
apstart64:
  # Remember (from bootothers), that our kernel stack pointer is
  # at the top of our temporary stack.
  popq %rax
  movq %rax, %rsp
  movq $0, %rbp
  call apmain
1:      jmp 1b
  
.code64
tramp64:
  # The linker thinks we are running at tramp64, but we're actually
  # running at PADDR(tramp64), so use an explicit calculation to
  # load and jump to the correct address.  %rax should hold the
  # physical address of the jmp target.
  movq $KERNBASE, %r11
  addq %r11, %rax
  jmp *%rax

# Initial stack
.comm stack, STACK

# Page tables.  See section 4.5 of 253668.pdf.
# We map the first GB of physical memory at 0 and at 1 TB (not GB) before
# the end of virtual memory.  At boot time we are using the mapping at 0
# but during ordinary execution we use the high mapping.
# The intent is that after bootstrap the kernel can expand this mapping
# to cover all the available physical memory.
# This would be easier if we could use the PS bit to create GB-sized entries
# and skip the pdt table, but not all chips support it, and QEMU doesn't.
.align 4096
pml4:
  .quad V2P_WO(pdpt) + PTE_P + PTE_W   // present, read/write
  .quad 0
  .space 4096 - 2*16
  .quad V2P_WO(pdpt) + PTE_P + PTE_W
  .quad 0

.align 4096
pdpt:
  .quad V2P_WO(pdt) + PTE_P + PTE_W
  .space 4096 - 8

.align 4096
pdt:
  // Filled in below.
  .space 4096

.code32
initpagetables:
  pushl %edi
  pushl %ecx
  pushl %eax

  // Set up 64-bit entry in %edx:%eax.
  // Base address 0, present, read/write, large page.
  movl $(0 | PTE_P | PTE_W | PTE_PS), %eax
  movl $0, %edx

  // Fill in 512 entries at pdt.
  movl $V2P_WO(pdt), %edi
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

# Initialize IA-32e mode.  See section 9.8.5 of 253668.pdf.
init32e:
  # Set CR4.PAE and CR4.PSE = 1.
  movl %cr4, %eax
  orl $0x30, %eax
  movl %eax, %cr4

  # Load CR3 with physical base address of level 4 page table.
  movl $V2P_WO(pml4), %eax
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

  ret

loadgdt:
  subl $8, %esp
  movl $V2P_WO(bootgdt), 4(%esp)
  movw $(8*NSEGS-1), 2(%esp)
  lgdt 2(%esp)
  addl $8, %esp

  movl $KDSEG, %eax  // data segment selector
  movw %ax, %ds
  movw %ax, %es
  movw %ax, %ss
  movl $0, %eax  // null segment selector
  movw %ax, %fs
  movw %ax, %gs

  ret
