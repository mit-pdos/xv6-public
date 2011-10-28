#
# XXX compiling user progs with -mcmodel=kernel
#

# Custom config file?  Otherwise use defaults.
-include config.mk
TOOLPREFIX ?= x86_64-jos-elf-
QEMU 	   ?= qemu-system-x86_64
QEMUSMP	   ?= 4
QEMUSRC    ?= ../mtrace
MTRACE	   ?= $(QEMU)

CC = $(TOOLPREFIX)clang
CC = $(TOOLPREFIX)gcc
AS = $(TOOLPREFIX)gas
LD = $(TOOLPREFIX)ld
NM = $(TOOLPREFIX)nm
OBJCOPY = $(TOOLPREFIX)objcopy

CFLAGS = -fno-pic -static -fno-builtin -fno-strict-aliasing -O2 -Wall -MD -ggdb \
	 -m64 -Werror -std=c99 -fms-extensions -mno-sse -mcmodel=kernel -I$(QEMUSRC)
CFLAGS += $(shell $(CC) -fno-stack-protector -E -x c /dev/null >/dev/null 2>&1 && echo -fno-stack-protector)
ASFLAGS = -m64 -gdwarf-2
LDFLAGS += -m elf_x86_64

OBJS = \
	bio.o \
	cga.o \
	condvar.o \
	console.o \
	exec.o \
	file.o \
	fs.o \
	ioapic.o \
	lapic.o \
	hwvm.o \
	hz.o \
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
	_cat \
	_init \
	_ls \
	_sh \

kernel: boot.o $(OBJS) initcode bootother fs.img
	$(LD) $(LDFLAGS) -T kernel.ld -z max-page-size=4096 -e start \
		-o $@ boot.o $(OBJS) -b binary initcode bootother fs.img

initcode: initcode.S
	$(CC) $(CFLAGS) -nostdinc -I. -c initcode.S
	$(LD) $(LDFLAGS) -N -e start -Ttext 0 -o initcode.out initcode.o
	$(OBJCOPY) -S -O binary initcode.out initcode

bootother: bootother.S
	$(CC) $(CFLAGS) -nostdinc -I. -c bootother.S
	$(LD) $(LDFLAGS) -N -e start -Ttext 0x7000 -o bootother.out bootother.o
	$(OBJCOPY) -S -O binary bootother.out bootother

xv6memfs.img: bootblock kernelmemfs
	dd if=/dev/zero of=xv6memfs.img count=10000
	dd if=bootblock of=xv6memfs.img conv=notrunc
	dd if=kernelmemfs of=xv6memfs.img seek=1 conv=notrunc

_%: %.o $(ULIB)
	$(LD) $(LDFLAGS) -N -e main -Ttext 0 -o $@ $^

mkfs: mkfs.c fs.h
	gcc -m32 -Werror -Wall -o mkfs mkfs.c

fs.img: mkfs README $(UPROGS)
	./mkfs fs.img README $(UPROGS)

mscan.syms: kernel
	$(NM) -S $< > $@

mscan.kernel: kernel
	cp $< $@

-include *.d

.PHONY: clean qemu gdb ud0

QEMUOPTS = -smp $(QEMUSMP) -m 512 -serial mon:stdio -nographic

qemu: kernel
	$(QEMU) $(QEMUOPTS) -kernel kernel
gdb: kernel
	$(QEMU) $(QEMUOPTS) -kernel kernel -S -s

##
## mtrace
##
MTRACEOPTS = -rtc clock=vm -mtrace-enable -mtrace-file mtrace.out \
	     -mtrace-quantum 100
mtrace: mscan.kernel mscan.syms 
	$(MTRACE) $(QEMUOPTS) $(MTRACEOPTS) -kernel kernel

mscan.out: mtrace.out $(QEMUSRC)/mtrace-tools/mscan
	$(QEMUSRC)/mtrace-tools/mscan > $@ || (rm -f $@; exit 2)

ud0: kernel
	rsync -avP kernel amsterdam.csail.mit.edu:/tftpboot/ud0/kernel.xv6

clean: 
	rm -f *.o *.d *.asm *.sym initcode kernel bootother mkfs fs.img
