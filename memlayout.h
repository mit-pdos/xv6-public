// Physical memory layout

// qemu -machine virt is set up like this,
// based on qemu's hw/riscv/virt.c:
//
// 00001000 -- boot ROM, provided by qemu
// 02000000 -- CLINT
// 0C000000 -- PLIC
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
#define UART0_IRQ 10

// local interrupt controller, which contains the timer.
#define CLINT 0x2000000L
#define CLINT_MSIP0 (CLINT + 0x0)
#define CLINT_MTIMECMP0 (CLINT + 0x4000)
#define CLINT_MTIME (CLINT + 0xBFF8)

// qemu puts programmable interrupt controller here.
#define PLIC 0x0c000000L

#define RAMDISK 0x88000000L

// the kernel expects there to be RAM
// for use by the kernel and user pages
// from physical address 0x80000000 to PHYSTOP.
#define KERNBASE 0x80000000L
#define PHYSTOP (KERNBASE + 128*1024*1024)

// map the trampoline page to the highest address,
// in both user and kernel space.
#define TRAMPOLINE (MAXVA - PGSIZE)
