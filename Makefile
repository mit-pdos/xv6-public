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

# XXX(sbw)
# -nostdinc -nostdinc++

COMFLAGS  = -static -fno-builtin -fno-strict-aliasing -O2 -Wall \
	    -g -MD -m64 -Werror -fms-extensions -mno-sse -mcx16 \
	    -mno-red-zone -Iinclude -I$(QEMUSRC) -fno-omit-frame-pointer \
	    -DHW_$(HW) -include param.h -include include/compiler.h -DXV6
COMFLAGS += $(shell $(CC) -fno-stack-protector -E -x c /dev/null >/dev/null 2>&1 && echo -fno-stack-protector)
CFLAGS   := $(COMFLAGS) -std=c99 $(CFLAGS)
CXXFLAGS := $(COMFLAGS) -std=c++0x -Wno-sign-compare -fno-exceptions -fno-rtti -fcheck-new $(CXXFLAGS)
ASFLAGS   = -Iinclude -m64 -gdwarf-2 -MD -DHW_$(HW) -include param.h
LDFLAGS   = -m elf_x86_64

all:

include net/Makefrag
include lib/Makefrag
include bin/Makefrag
include kernel/Makefrag
include tools/Makefrag
-include user/Makefrag.$(HW)

$(O)/%.o: %.c
	@echo "  CC     $@"
	$(Q)mkdir -p $(@D)
	$(Q)$(CC) $(CFLAGS) -c -o $@ $<

$(O)/%.o: %.cc
	@echo "  CXX    $@"
	$(Q)mkdir -p $(@D)
	$(Q)$(CXX) $(CXXFLAGS) $(XXFLAGS) -c -o $@ $<

$(O)/%.o: %.S
	@echo "  CC     $@"
	$(Q)mkdir -p $(@D)
	$(Q)$(CC) $(ASFLAGS) -c -o $@ $<

xv6memfs.img: bootblock kernelmemfs
	dd if=/dev/zero of=xv6memfs.img count=10000
	dd if=bootblock of=xv6memfs.img conv=notrunc
	dd if=kernelmemfs of=xv6memfs.img seek=1 conv=notrunc

$(O)/fs.img: $(O)/tools/mkfs README $(UPROGS)
	@echo "  MKFS   $@"
	$(Q)$(O)/tools/mkfs $@ README $(UPROGS)

.PRECIOUS: $(O)/%.o
.PHONY: clean qemu gdb rsync

##
## qemu
##
QEMUOPTS = -smp $(QEMUSMP) -m 512 -serial mon:stdio -nographic \
	-net user -net nic,model=e1000 \
	-redir tcp:2323::23 -redir tcp:8080::80

qemu: $(KERN)
	$(QEMU) $(QEMUOPTS) -kernel $(KERN)
gdb: $(KERN)
	$(QEMU) $(QEMUOPTS) -kernel $(KERN) -S -s

##
## mtrace
##
mscan.syms: $(KERN)
	$(NM) -S $< > $@

mscan.kern: $(KERN)
	cp $< $@

MTRACEOPTS = -rtc clock=vm -mtrace-enable -mtrace-file mtrace.out \
	     -mtrace-quantum 100
mtrace.out: mscan.kern mscan.syms 
	$(Q)rm -f mtrace.out
	$(MTRACE) $(QEMUOPTS) $(MTRACEOPTS) -kernel mscan.kern

mscan.out: $(QEMUSRC)/mtrace-tools/mscan mtrace.out
	$(QEMUSRC)/mtrace-tools/mscan > $@ || (rm -f $@; exit 2)

mscan.sorted: mscan.out $(QEMUSRC)/mtrace-tools/sersec-sort
	$(QEMUSRC)/mtrace-tools/sersec-sort < $< > $@

rsync: $(KERN)
	rsync -avP $(KERN) amsterdam.csail.mit.edu:/tftpboot/$(HW)/kernel.xv6

reboot:
	ipmitool -I lanplus -A MD5 -H $(HW)adm.csail.mit.edu -U ADMIN -f .passwd.$(HW) power reset

clean: 
	rm -fr $(O)
