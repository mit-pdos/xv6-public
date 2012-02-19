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
O  	   = o.$(HW)

ifdef USE_CLANG
CC  = $(TOOLPREFIX)clang
CXX = $(TOOLPREFIX)clang++ 
CXXFLAGS = -Wno-delete-non-virtual-dtor -Wno-gnu-designator
CFLAGS   = -no-integrated-as
else
CC  = $(TOOLPREFIX)gcc
CXX = $(TOOLPREFIX)g++
CXXFLAGS =
CFLAGS   =
endif

LD = $(TOOLPREFIX)ld
NM = $(TOOLPREFIX)nm
OBJCOPY = $(TOOLPREFIX)objcopy
STRIP = $(TOOLPREFIX)strip

COMFLAGS  = -static -fno-builtin -fno-strict-aliasing -O2 -Wall \
	    -g -MD -m64 -Werror -fms-extensions -mno-sse \
	    -mcmodel=large -mno-red-zone -I$(QEMUSRC) -fno-omit-frame-pointer \
	    -DHW_$(HW) -include param.h -include compiler.h -DXV6
COMFLAGS += $(shell $(CC) -fno-stack-protector -E -x c /dev/null >/dev/null 2>&1 && echo -fno-stack-protector)
CFLAGS   := $(COMFLAGS) -std=c99 $(CFLAGS)
CXXFLAGS := $(COMFLAGS) -std=c++0x -Wno-sign-compare -fno-exceptions -fno-rtti $(CXXFLAGS)
ASFLAGS   = -m64 -gdwarf-2 -MD
LDFLAGS   = -m elf_x86_64

OBJS = \
	async.o \
	bio.o \
	bootdata.o \
	cga.o \
	cilk.o \
	condvar.o \
	console.o \
	cpprt.o \
	crange.o \
	e1000.o \
	exec.o \
	file.o \
	fmt.o \
	fs.o \
	ioapic.o \
	lapic.o \
	hwvm.o \
	hz.o \
	kalloc.o \
	kmalloc.o \
	kbd.o \
	main.o \
	memide.o \
	mp.o \
	net.o \
	pci.o \
	picirq.o \
	pipe.o \
	proc.o \
	prof.o \
	gc.o \
	rnd.o \
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

ULIB = ulib.o usys.o printf.o umalloc.o uthread.o fmt.o
ULIB := $(addprefix $(O)/, $(ULIB))

UPROGS= \
	_cat \
	_echo \
	_init \
	_forkexectree \
	_forkexecbench \
	_forktree \
	_login \
	_ls \
	_mapbench \
	_maptest \
	_sh \
	_thrtest \
	_halt \
	_time \
	_sleep \
	_dirbench \
	_usertests \
	_lockstat \
	_preadtest \
	_perf
UPROGS := $(addprefix $(O)/, $(UPROGS))

all: $(O)/kernel

include net.mk

$(O)/kernel: $(O) $(O)/boot.o $(OBJS)
	@echo "  LD     $@"
	$(Q)$(LD) $(LDFLAGS) -T kernel.ld -z max-page-size=4096 -e start \
		-o $@ $(O)/boot.o $(OBJS)

$(O):
	$(Q)mkdir $(O)
	$(Q)mkdir $(O)/net

$(O)/%.o: %.c
	@echo "  CC     $@"
	$(Q)$(CC) $(CFLAGS) -c -o $@ $<

$(O)/%.o: %.cc
	@echo "  CXX    $@"
	$(Q)$(CXX) $(CXXFLAGS) -c -o $@ $<

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

$(O)/_%.unstripped: $(O)/%.o $(ULIB)
	@echo "  LD     $@"
	$(Q)$(LD) $(LDFLAGS) -N -e main -Ttext 0x100000 -o $@ $^

$(O)/_%: $(O)/_%.unstripped
	@echo "  STRIP  $@"
	$(Q)$(STRIP) -o $@ $<

$(O)/mkfs: mkfs.c fs.h
	gcc -m32 -Werror -Wall -o $@ mkfs.c

$(O)/perf-report: perf-report.cc sampler.h
	$(CXX) -std=c++0x -m64 -Werror -Wall -o $@ perf-report.cc

$(O)/fs.img: $(O)/mkfs README $(UPROGS)
	@echo "  MKFS   $@"
	$(Q)$(O)/mkfs $@ README $(UPROGS)

mscan.syms: $(O)/kernel
	$(NM) -S $< > $@

mscan.kern: $(O)/kernel
	cp $< $@

-include *.d
-include $(O)/*.d

.PRECIOUS: $(O)/%.o $(O)/_%.unstripped
.PHONY: clean qemu gdb rsync

##
## qemu
##
QEMUOPTS = -smp $(QEMUSMP) -m 512 -serial mon:stdio -nographic \
	-net user -net nic,model=e1000 \
	-redir tcp:2323::23 -redir tcp:8080::80

qemu: $(O)/kernel
	$(QEMU) $(QEMUOPTS) -kernel $(O)/kernel
gdb: $(O)/kernel
	$(QEMU) $(QEMUOPTS) -kernel $(O)/kernel -S -s

##
## mtrace
##
MTRACEOPTS = -rtc clock=vm -mtrace-enable -mtrace-file mtrace.out \
	     -mtrace-quantum 100
mtrace.out: mscan.kern mscan.syms 
	$(Q)rm -f mtrace.out
	$(MTRACE) $(QEMUOPTS) $(MTRACEOPTS) -kernel mscan.kern

mscan.out: $(QEMUSRC)/mtrace-tools/mscan mtrace.out
	$(QEMUSRC)/mtrace-tools/mscan > $@ || (rm -f $@; exit 2)

mscan.sorted: mscan.out $(QEMUSRC)/mtrace-tools/sersec-sort
	$(QEMUSRC)/mtrace-tools/sersec-sort < $< > $@

rsync: $(O)/kernel
	rsync -avP $(O)/kernel amsterdam.csail.mit.edu:/tftpboot/$(HW)/kernel.xv6

clean: 
	rm -fr $(O)
