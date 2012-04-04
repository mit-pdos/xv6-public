# Custom config file?  Otherwise use defaults.
-include config.mk
Q          ?= @
TOOLPREFIX ?= x86_64-jos-elf-
QEMU 	   ?= qemu-system-x86_64
QEMUSMP	   ?= 8
QEMUSRC    ?= ../mtrace
MTRACE	   ?= $(QEMU)
HW	   ?= qemu
EXCEPTIONS ?= y
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
CFLAGS =
ASFLAGS =
endif

LD = $(TOOLPREFIX)ld
NM = $(TOOLPREFIX)nm
OBJCOPY = $(TOOLPREFIX)objcopy
STRIP = $(TOOLPREFIX)strip

INCLUDES  = -iquote include -Istdinc -I$(QEMUSRC) -include param.h -include include/compiler.h
COMFLAGS  = -static -g -MD -m64 -O3 -Wall -Werror -DHW_$(HW) -DXV6 \
	    -fno-builtin -fno-strict-aliasing -fno-omit-frame-pointer -fms-extensions \
	    -mno-sse -mcx16 -mno-red-zone $(INCLUDES)
COMFLAGS += $(shell $(CC) -fno-stack-protector -E -x c /dev/null >/dev/null 2>&1 && echo -fno-stack-protector) -nostdinc -I$(shell $(CC) -print-file-name=include)
CFLAGS   := $(COMFLAGS) -std=c99 $(CFLAGS)
CXXFLAGS := $(COMFLAGS) -std=c++0x -Wno-sign-compare -nostdinc++ $(CXXFLAGS)
ASFLAGS   = -Iinclude -I$(O)/include -m64 -gdwarf-2 -MD -DHW_$(HW) -include param.h -Wa,--divide 
LDFLAGS   = -m elf_x86_64

ALL := 
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
	$(Q)$(CXX) $(CXXFLAGS) -c -o $@ $<

$(O)/%.o: %.S
	@echo "  CC     $@" $(ASFLAGS)
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
	$(NM) -C -S $< > $@

mscan.kern: $(KERN)
	cp $< $@

MTRACEOPTS = -rtc clock=vm -mtrace-enable -mtrace-file mtrace.out \
	     -mtrace-quantum 100
mtrace.out: mscan.kern mscan.syms 
	$(Q)rm -f mtrace.out
	$(MTRACE) $(QEMUOPTS) $(MTRACEOPTS) -kernel mscan.kern
.PHONY: mtrace.out

mscan.out: $(QEMUSRC)/mtrace-tools/mscan mtrace.out
	$(QEMUSRC)/mtrace-tools/mscan > $@ || (rm -f $@; exit 2)

mscan.sorted: mscan.out $(QEMUSRC)/mtrace-tools/sersec-sort
	$(QEMUSRC)/mtrace-tools/sersec-sort < $< > $@

rsync: $(KERN)
	rsync -avP $(KERN) amsterdam.csail.mit.edu:/tftpboot/$(HW)/kernel.xv6

reboot:
	ipmitool -I lanplus -A MD5 -H $(HW)adm.csail.mit.edu -U ADMIN -f .passwd.$(HW) power reset

bench:
	/bin/echo -ne "xv6\\nbench\\nexit\\n" | nc $(HW).csail.mit.edu 23

clean: 
	rm -fr $(O)

all:	$(ALL)

