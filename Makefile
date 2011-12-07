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
HW	   ?= qemu

O  = o.$(HW)
CC = $(TOOLPREFIX)gcc
AS = $(TOOLPREFIX)gas
LD = $(TOOLPREFIX)ld
NM = $(TOOLPREFIX)nm
OBJCOPY = $(TOOLPREFIX)objcopy

CFLAGS = -fno-pic -static -fno-builtin -fno-strict-aliasing -O2 -Wall -MD -ggdb \
	 -m64 -Werror -std=c99 -fms-extensions -mno-sse -mcmodel=large -I$(QEMUSRC) \
	 -fno-omit-frame-pointer -DHW_$(HW) -include param.h
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
	prof.o \
	gc.o \
	sampler.o \
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
	wq.o \
	incbin.o
OBJS := $(addprefix $(O)/, $(OBJS))

ULIB = ulib.o usys.o printf.o umalloc.o uthread.o
ULIB := $(addprefix $(O)/, $(ULIB))

UPROGS= \
	_cat \
	_init \
	_forkexectree \
	_forkexecbench \
	_forktree \
	_ls \
	_mapbench \
	_maptest \
	_sh \
	_thrtest
UPROGS := $(addprefix $(O)/, $(UPROGS))

$(O)/kernel: $(O) $(O)/boot.o $(OBJS)
	@echo "  LD     $@"
	$(Q)$(LD) $(LDFLAGS) -T kernel.ld -z max-page-size=4096 -e start \
		-o $@ $(O)/boot.o $(OBJS)

$(O):
	$(Q)mkdir $(O)

$(O)/%.o: %.c
	@echo "  CC     $@"
	$(Q)$(CC) $(CFLAGS) -c -o $@ $<

$(O)/incbin.o: ASFLAGS+=-DMAKE_OUT=$(O)
$(O)/incbin.o: $(O)/initcode $(O)/bootother $(O)/fs.img
$(O)/%.o: %.S
	@echo "  CC     $@"
	$(Q)$(CC) $(ASFLAGS) -c -o $@ $<

$(O)/initcode: TTEXT = 0x0
$(O)/bootother: TTEXT = 0x7000
$(O)/%: %.S
	@echo "  CC     $@"
	$(Q)$(CC) $(CFLAGS) -nostdinc -I. -c $< -o $@.o
	$(Q)$(LD) $(LDFLAGS) -N -e start -Ttext $(TTEXT) -o $@.out $@.o
	$(Q)$(OBJCOPY) -S -O binary $@.out $@

xv6memfs.img: bootblock kernelmemfs
	dd if=/dev/zero of=xv6memfs.img count=10000
	dd if=bootblock of=xv6memfs.img conv=notrunc
	dd if=kernelmemfs of=xv6memfs.img seek=1 conv=notrunc

$(O)/_%: $(O)/%.o $(ULIB)
	@echo "  LD     $@"
	$(Q)$(LD) $(LDFLAGS) -N -e main -Ttext 0 -o $@ $^

$(O)/mkfs: mkfs.c fs.h
	gcc -m32 -Werror -Wall -o $@ mkfs.c

$(O)/fs.img: $(O)/mkfs README $(UPROGS)
	@echo "  MKFS   $@"
	$(Q)$(O)/mkfs $@ README $(UPROGS)

$(O)/mscan.syms: $(O)/kernel
	$(NM) -S $< > $@

$(O)/mscan.kern: $(O)/kernel
	cp $< $@

-include *.d
-include $(O)/*.d

.PRECIOUS: $(O)/%.o
.PHONY: clean qemu gdb rsync

##
## qemu
##
QEMUOPTS = -smp $(QEMUSMP) -m 512 -serial mon:stdio -nographic

qemu: $(O)/kernel
	$(QEMU) $(QEMUOPTS) -kernel $(O)/kernel
gdb: $(O)/kernel
	$(QEMU) $(QEMUOPTS) -kernel $(O)/kernel -S -s

##
## mtrace
##
MTRACEOPTS = -rtc clock=vm -mtrace-enable -mtrace-file mtrace.out \
	     -mtrace-quantum 100
mtrace.out: $(O)/mscan.kern $(O)/mscan.syms 
	$(Q)rm -f mtrace.out
	$(MTRACE) $(QEMUOPTS) $(MTRACEOPTS) -kernel $(O)/mscan.kern

mscan.out: $(QEMUSRC)/mtrace-tools/mscan mtrace.out
	$(QEMUSRC)/mtrace-tools/mscan > $@ || (rm -f $@; exit 2)

mscan.sorted: mscan.out $(QEMUSRC)/mtrace-tools/sersec-sort
	$(QEMUSRC)/mtrace-tools/sersec-sort < $< > $@

rsync: $(O)/kernel
	rsync -avP $(O)/kernel amsterdam.csail.mit.edu:/tftpboot/$(HW)/kernel.xv6

clean: 
	rm -fr $(O)
