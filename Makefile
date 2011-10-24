OBJS = \
	asm.o \
	bio.o \
	cga.o \
	condvar.o \
	console.o \
	fs.o \
	ide.o \
	lapic.o \
	kalloc.o \
	main.o \
	mp.o \
	ns.o \
	proc.o \
	rcu.o \
	spinlock.o \
	string.o \
	uart.o \
	vm.o \
	trap.o \
	trapasm.o

# Custom config file?  Set the default below..
-include config.mk

TOOLPREFIX ?= x86_64-jos-elf-
QEMU ?= qemu-system-x86_64
CPUS ?= 2

NM = $(TOOLPREFIX)nm
CC = $(TOOLPREFIX)gcc
AS = $(TOOLPREFIX)gas
LD = $(TOOLPREFIX)ld
OBJCOPY = $(TOOLPREFIX)objcopy
OBJDUMP = $(TOOLPREFIX)objdump
CFLAGS = -fno-pic -static -fno-builtin -fno-strict-aliasing -O2 -Wall -MD -ggdb -m64 \
         -Werror -std=c99 -fms-extensions -mno-sse -mcmodel=kernel
CFLAGS += $(shell $(CC) -fno-stack-protector -E -x c /dev/null >/dev/null 2>&1 && echo -fno-stack-protector)
ASFLAGS = -m64 -gdwarf-2
LDFLAGS += -m elf_x86_64

kernel: boot.o $(OBJS) initcode
	$(LD) $(LDFLAGS) -T kernel.ld -z max-page-size=4096 -e start \
		-o $@ boot.o $(OBJS) -b binary initcode

initcode: initcode.S
	$(CC) $(CFLAGS) -nostdinc -I. -c initcode.S
	$(LD) $(LDFLAGS) -N -e start -Ttext 0 -o initcode.out initcode.o
	$(OBJCOPY) -S -O binary initcode.out initcode
	$(OBJDUMP) -S initcode.o > initcode.asm

xv6memfs.img: bootblock kernelmemfs
	dd if=/dev/zero of=xv6memfs.img count=10000
	dd if=bootblock of=xv6memfs.img conv=notrunc
	dd if=kernelmemfs of=xv6memfs.img seek=1 conv=notrunc

-include *.d

.PHONY: clean qemu ud0

clean: 
	rm -f *.o *.d *.asm *.sym initcode kernel

QEMUOPTS = -smp $(CPUS) -m 512  -nographic
qemu: kernel
	$(QEMU) $(QEMUOPTS) -kernel kernel

ud0: kernel
	rsync -avP kernel amsterdam.csail.mit.edu:/tftpboot/ud0/kernel.xv6
