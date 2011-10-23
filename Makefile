-include config.mk

OBJS = \
	asm.o \
	cga.o \
	console.o \
	main.o \
	mp.o \
	spinlock.o \
	string.o \
	uart.o \
	trap.o

# Cross-compiling (e.g., on Mac OS X)
TOOLPREFIX ?= x86_64-jos-elf-

# If the makefile can't find QEMU, specify its path here
QEMU = qemu-system-x86_64

NM = $(TOOLPREFIX)nm
CC = $(TOOLPREFIX)gcc
AS = $(TOOLPREFIX)gas
LD = $(TOOLPREFIX)ld
OBJCOPY = $(TOOLPREFIX)objcopy
OBJDUMP = $(TOOLPREFIX)objdump
CFLAGS = -fno-pic -static -fno-builtin -fno-strict-aliasing -O2 -Wall -MD -ggdb -m64 \
         -Werror -fms-extensions -mno-sse -mcmodel=kernel
CFLAGS += $(shell $(CC) -fno-stack-protector -E -x c /dev/null >/dev/null 2>&1 && echo -fno-stack-protector)
ASFLAGS = -m64 -gdwarf-2
LDFLAGS += -m elf_x86_64

kernel: boot.o $(OBJS)
	$(LD) $(LDFLAGS) -T kernel.ld -z max-page-size=4096 -e start -o $@ boot.o $(OBJS)
	$(OBJDUMP) -S $@ >$@.asm

xv6memfs.img: bootblock kernelmemfs
	dd if=/dev/zero of=xv6memfs.img count=10000
	dd if=bootblock of=xv6memfs.img conv=notrunc
	dd if=kernelmemfs of=xv6memfs.img seek=1 conv=notrunc

-include *.d

.PHONY: clean qemu

clean: 
	rm -f *.tex *.dvi *.idx *.aux *.log *.ind *.ilg \
	*.o *.d *.asm *.sym vectors.S parport.out \
	bootblock kernel xv6.img fs.img mkfs

ifndef CPUS
CPUS := 2
endif
QEMUOPTS = -smp $(CPUS) -m 512

qemu: kernel
	$(QEMU) $(QEMUOPTS) -kernel kernel -nographic
