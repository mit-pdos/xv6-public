#
# XXX compiling user progs with -mcmodel=kernel
#

# Custom config file?  Otherwise use defaults.
-include config.mk
Q          ?= @
TOOLPREFIX ?= x86_64-jos-elf-
QEMU 	   ?= qemu-system-x86_64
QEMUSMP	   ?= 4
QEMUSRC    ?= ../mtrace
MTRACE	   ?= $(QEMU)

CC = $(TOOLPREFIX)gcc
AS = $(TOOLPREFIX)gas
LD = $(TOOLPREFIX)ld
NM = $(TOOLPREFIX)nm
OBJCOPY = $(TOOLPREFIX)objcopy

CFLAGS = -fno-pic -static -fno-builtin -fno-strict-aliasing -O2 -Wall -MD -ggdb \
	 -m64 -Werror -std=c99 -fms-extensions -mno-sse -mcmodel=kernel -I$(QEMUSRC) \
	 -fno-omit-frame-pointer
CFLAGS += $(shell $(CC) -fno-stack-protector -E -x c /dev/null >/dev/null 2>&1 && echo -fno-stack-protector)
ASFLAGS = -m64 -gdwarf-2
LDFLAGS += -m elf_x86_64

OBJS = \
	bio.o \
	bonsai.o \
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
	sched.o \
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

ULIB = ulib.o usys.o printf.o umalloc.o uthread.o

UPROGS= \
	_cat \
	_init \
	_forktree \
	_ls \
	_mapbench \
	_maptest \
	_sh \
	_thrtest \

%.o: %.c
	@echo "  CC     $@"
	$(Q)$(CC) $(CFLAGS) -c -o $@ $<

%.o: %.S
	@echo "  CC     $@"
	$(Q)$(CC) $(ASFLAGS) -c -o $@ $<

kernel: boot.o $(OBJS) initcode bootother fs.img
	@echo "  LD     $@"
	$(Q)$(LD) $(LDFLAGS) -T kernel.ld -z max-page-size=4096 -e start \
		-o $@ boot.o $(OBJS) -b binary initcode bootother fs.img

initcode: TTEXT = 0x0
bootother: TTEXT = 0x7000
%: %.S
	@echo "  CC     $@"
	$(Q)$(CC) $(CFLAGS) -nostdinc -I. -c $@.S
	$(Q)$(LD) $(LDFLAGS) -N -e start -Ttext $(TTEXT) -o $@.out $@.o
	$(Q)$(OBJCOPY) -S -O binary $@.out $@

xv6memfs.img: bootblock kernelmemfs
	dd if=/dev/zero of=xv6memfs.img count=10000
	dd if=bootblock of=xv6memfs.img conv=notrunc
	dd if=kernelmemfs of=xv6memfs.img seek=1 conv=notrunc

_%: %.o $(ULIB)
	@echo "  LD     $@"
	$(Q)$(LD) $(LDFLAGS) -N -e main -Ttext 0 -o $@ $^

mkfs: mkfs.c fs.h
	gcc -m32 -Werror -Wall -o mkfs mkfs.c

fs.img: mkfs README $(UPROGS)
	./mkfs fs.img README $(UPROGS)

mscan.syms: kernel
	$(NM) -S $< > $@

mscan.kern: kernel
	cp $< $@

-include *.d

.PHONY: clean qemu gdb mtrace ud0

##
## qemu
##
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
mtrace: mscan.kern mscan.syms 
	$(MTRACE) $(QEMUOPTS) $(MTRACEOPTS) -kernel mscan.kern

mscan.out: $(QEMUSRC)/mtrace-tools/mscan
	$(QEMUSRC)/mtrace-tools/mscan > $@ || (rm -f $@; exit 2)

mscan.sorted: mscan.out $(QEMUSRC)/mtrace-tools/sersec-sort
	$(QEMUSRC)/mtrace-tools/sersec-sort < $< > $@

ud0: kernel
	rsync -avP kernel amsterdam.csail.mit.edu:/tftpboot/ud0/kernel.xv6

clean: 
	rm -f *.o *.d *.asm *.sym initcode kernel bootother mkfs fs.img _*
