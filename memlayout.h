// Physical memory layout

// qemu -machine virt is set up like this:
// 00001000 -- boot ROM, provided by qemu
// 10000000 -- uart0 registers
// 80000000 -- boot ROM jumps here in machine mode
//             -kernel loads the kernel here
// 88000000 -- -initrd fs.img ramdisk image.
// unused RAM after 80000000.

// the kernel uses physical memory thus:
// 80000000 -- entry.S, then kernel text and data
// end -- start of kernel page allocation area
// PHYSTOP -- end RAM used by the kernel

// qemu puts UART registers here in physical memory.
#define UART0 0x10000000L

// qemu puts programmable interrupt controller here.
#define PLIC 0x0c000000L

#define RAMDISK 0x88000000

// the kernel expects there to be RAM
// for use by the kernel and user pages
// from physical address 0x80000000 to PHYSTOP.
#define KERNBASE 0x80000000L
#define PHYSTOP (KERNBASE + 64*1024*1024)

// map the trampoline page to the highest address,
// in both user and kernel space.
#define TRAMPOLINE (MAXVA - PGSIZE)
