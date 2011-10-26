#
# XXX compiling user progs with -mcmodel=kernel
#

# Custom config file?  Set the default below..
-include config.mk

TOOLPREFIX ?= x86_64-jos-elf-
QEMU ?= qemu-system-x86_64
CPUS ?= 4

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

OBJS = \
	asm.o \
	bio.o \
	cga.o \
	condvar.o \
	console.o \
	exec.o \
	file.o \
	fs.o \
	ioapic.o \
	lapic.o \
	kalloc.o \
	kbd.o \
	main.o \
	memide.o \
	mp.o \
	ns.o \
	picirq.o \
	pipe.o \
	proc.o \
	rcu.o \
	spinlock.o \
	swtch.o \
	string.o \
	syscall.o \
	sysfile.o \
	sysproc.o \
	uart.o \
	vm.o \
	trap.o \
	trapasm.o \

ULIB = ulib.o usys.o printf.o umalloc.o

UPROGS= \
	_init \
	_sh \
	_ls \

kernel: boot.o $(OBJS) initcode bootother fs.img
	$(LD) $(LDFLAGS) -T kernel.ld -z max-page-size=4096 -e start \
		-o $@ boot.o $(OBJS) -b binary initcode bootother fs.img

initcode: initcode.S
	$(CC) $(CFLAGS) -nostdinc -I. -c initcode.S
	$(LD) $(LDFLAGS) -N -e start -Ttext 0 -o initcode.out initcode.o
	$(OBJCOPY) -S -O binary initcode.out initcode
	$(OBJDUMP) -S initcode.o > initcode.asm

bootother: bootother.S
	$(CC) $(CFLAGS) -nostdinc -I. -c bootother.S
	$(LD) $(LDFLAGS) -N -e start -Ttext 0x7000 -o bootother.out bootother.o
	$(OBJCOPY) -S -O binary bootother.out bootother
	$(OBJDUMP) -S bootother.out > bootother.asm

xv6memfs.img: bootblock kernelmemfs
	dd if=/dev/zero of=xv6memfs.img count=10000
	dd if=bootblock of=xv6memfs.img conv=notrunc
	dd if=kernelmemfs of=xv6memfs.img seek=1 conv=notrunc

_%: %.o $(ULIB)
	$(LD) $(LDFLAGS) -N -e main -Ttext 0 -o $@ $^
	$(OBJDUMP) -S $@ > $*.asm
	$(OBJDUMP) -t $@ | sed '1,/SYMBOL TABLE/d; s/ .* / /; /^$$/d' > $*.sym

mkfs: mkfs.c fs.h
	gcc -m32 -Werror -Wall -o mkfs mkfs.c

fs.img: mkfs README $(UPROGS)
	./mkfs fs.img README $(UPROGS)

-include *.d

.PHONY: clean qemu gdb ud0

clean: 
	rm -f *.o *.d *.asm *.sym initcode kernel bootother mkfs fs.img

QEMUOPTS = -smp $(CPUS) -m 512 -nographic
qemu: kernel
	$(QEMU) $(QEMUOPTS) -kernel kernel
gdb: kernel
	$(QEMU) $(QEMUOPTS) -kernel kernel -S -s

ud0: kernel
	rsync -avP kernel amsterdam.csail.mit.edu:/tftpboot/ud0/kernel.xv6
