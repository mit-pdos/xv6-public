# The make utility automatically determines which pieces of a large program
# need to be recompiled, and issues commands to recompile them. This file tells
# make how to compile and link the program. This makefile also tells make how
# to run miscellaneous commands when explicitly asked (for example, to run an
# emulator after building the correct filesystem image, or to remove certain
# files as a clean-up operation).
#
# http://www.gnu.org/software/make/manual/

# Default make target
.PHONY: all
all: xv6.img fs.img

################################################################################
# Build options
################################################################################

# delete target if error building it
.DELETE_ON_ERROR:

# C compiler options
# http://gcc.gnu.org/onlinedocs/gcc-4.4.6/gcc/Invoking-GCC.html
CC = gcc
# enable extra warnings
CFLAGS += -Wall
# treat warnings as errors
CFLAGS += -Werror
# produce debugging information for use by gdb
CFLAGS += -ggdb

# uncomment to enable optimizations. improves performance, but may make
# debugging more difficult
#CFLAGS += -O2

# C Preprocessor
CPP := cpp

# Assembler options
# http://sourceware.org/binutils/docs/as/Invoking.html
AS := gcc
ASFLAGS += -ggdb # produce debugging information for use by gdb

# Linker options
# http://sourceware.org/binutils/docs/ld/Options.html
LD := ld

OBJCOPY := objcopy

OBJDUMP := objdump

################################################################################
# Emulator Options
################################################################################

# Cross-compiling (e.g., on Mac OS X)
# TOOLPREFIX = i386-jos-elf

# Using native tools (e.g., on X86 Linux)
#TOOLPREFIX = 

# Try to infer the correct TOOLPREFIX if not set
ifndef TOOLPREFIX
TOOLPREFIX := $(shell if i386-jos-elf-objdump -i 2>&1 | grep '^elf32-i386$$' >/dev/null 2>&1; \
	then echo 'i386-jos-elf-'; \
	elif objdump -i 2>&1 | grep 'elf32-i386' >/dev/null 2>&1; \
	then echo ''; \
	else echo "***" 1>&2; \
	echo "*** Error: Couldn't find an i386-*-elf version of GCC/binutils." 1>&2; \
	echo "*** Is the directory with i386-jos-elf-gcc in your PATH?" 1>&2; \
	echo "*** If your i386-*-elf toolchain is installed with a command" 1>&2; \
	echo "*** prefix other than 'i386-jos-elf-', set your TOOLPREFIX" 1>&2; \
	echo "*** environment variable to that prefix and run 'make' again." 1>&2; \
	echo "*** To turn off this error, run 'gmake TOOLPREFIX= ...'." 1>&2; \
	echo "***" 1>&2; exit 1; fi)
endif

# If the makefile can't find QEMU, specify its path here
# QEMU = qemu-system-i386

# Try to infer the correct QEMU
ifndef QEMU
QEMU = $(shell if which qemu > /dev/null; \
	then echo qemu; exit; \
	elif which qemu-system-i386 > /dev/null; \
	then echo qemu-system-i386; exit; \
	elif which qemu-system-x86_64 > /dev/null; \
	then echo qemu-system-x86_64; exit; \
	else \
	qemu=/Applications/Q.app/Contents/MacOS/i386-softmmu.app/Contents/MacOS/i386-softmmu; \
	if test -x $$qemu; then echo $$qemu; exit; fi; fi; \
	echo "***" 1>&2; \
	echo "*** Error: Couldn't find a working QEMU executable." 1>&2; \
	echo "*** Is the directory containing the qemu binary in your PATH" 1>&2; \
	echo "*** or have you tried setting the QEMU variable in Makefile?" 1>&2; \
	echo "***" 1>&2; exit 1)
endif


# try to generate a unique GDB port
GDBPORT := $(shell expr `id -u` % 5000 + 25000)

# QEMU's gdb stub command line changed in 0.11
QEMUGDB := $(shell if $(QEMU) -help | grep -q '^-gdb'; \
	then echo "-gdb tcp::$(GDBPORT)"; \
	else echo "-s -p $(GDBPORT)"; fi)

# number of CPUs to emulate in QEMU
ifndef CPUS
CPUS := 2
endif

QEMUOPTS := -hdb fs.img xv6.img -smp $(CPUS)

################################################################################
# Main Targets
################################################################################

# Kernel objects
KERNEL_OBJECTS := \
	bio.o\
	console.o\
	exec.o\
	file.o\
	fs.o\
	ide.o\
	ioapic.o\
	kalloc.o\
	kbd.o\
	lapic.o\
	main.o\
	mp.o\
	picirq.o\
	pipe.o\
	proc.o\
	spinlock.o\
	string.o\
	swtch.o\
	syscall.o\
	sysfile.o\
	sysproc.o\
	timer.o\
	trapasm.o\
	trap.o\
	uart.o\
	vectors.o\
	vm.o\

KERNEL_OBJECTS := $(KERNEL_OBJECTS)

# Objects compiled in a special way for boot or alignment
KERNEL_SPECIAL_OBJECTS := \
	bootmain.o\
	bootasm.o\
	multiboot.o\
	data.o\
	bootother.o\
	initcode.o

KERNEL_SPECIAL_OBJECTS := $(KERNEL_SPECIAL_OBJECTS)

# Make dependency files
KERNEL_DEPS := \
	$(KERNEL_OBJECTS:.o=.d) \
	$(KERNEL_SPECIAL_OBJECTS:.o=.d)

# All generated files
KERNEL_CLEAN := \
	$(KERNEL_OBJECTS) \
	$(KERNEL_SPECIAL_OBJECTS) \
	$(KERNEL_DEPS) \
	vectors.S\
	bootblock\
	bootblock.out\
	bootother.out\
	initcode.out\
	kernel\
	bootother\
	initcode\
	xv6.img

# add include dir to search path for headers
KERNEL_CPPFLAGS += -I include
# do not search standard system paths for headers
KERNEL_CPPFLAGS += -nostdinc
# disable PIC (position independent code)
KERNEL_CFLAGS += -fno-pic
# do not use GCC builtin funtions (used to optimize common functions)
KERNEL_CFLAGS += -fno-builtin
# disable some optimizations that assume one object will not alias an object
# of a different type
KERNEL_CFLAGS += -fno-strict-aliasing
# disable stack smashing check
KERNEL_CFLAGS += -fno-stack-protector
# generate code for 32-bit environment
KERNEL_CFLAGS += -m32

KERNEL_ASFLAGS += $(KERNEL_CFLAGS)

# FreeBSD ld wants ``elf_i386_fbsd''
KERNEL_LDFLAGS += -m $(shell $(LD) -V | grep elf_i386 2>/dev/null)
# do not link with the host standard library files
KERNEL_LDFLAGS += -nostdlib
# use simple contiguous section layout and do not use dynamic linking
KERNEL_LDFLAGS += --omagic

# bootable disk image
xv6.img: bootblock kernel
	dd if=/dev/zero of=xv6.img count=10000
	dd if=bootblock of=xv6.img conv=notrunc
	dd if=kernel of=xv6.img seek=1 conv=notrunc

kernel:	\
		$(KERNEL_OBJECTS) multiboot.o data.o bootother initcode
	$(LD) $(LDFLAGS) $(KERNEL_LDFLAGS) \
		--section-start=.text=0x100000 --entry=main --output=kernel \
		multiboot.o data.o $(KERNEL_OBJECTS) \
		-b binary initcode bootother

# bootblock is optimized for space
bootmain.o: bootmain.c
	$(CC) $(CPPFLAGS) $(KERNEL_CPPFLAGS) $(CFLAGS) $(KERNEL_CFLAGS) \
		 -Os -c -o $@ $<

bootblock: bootasm.o bootmain.o
	$(LD) $(LDFLAGS) $(KERNEL_LDFLAGS) \
		--entry=start --section-start=.text=0x7C00 \
		--output=bootblock.out bootasm.o bootmain.o
	$(OBJCOPY) -S -O binary -j .text bootblock.out bootblock
	./sign.pl bootblock

bootother: bootother.o
	$(LD) $(LDFLAGS) $(KERNEL_LDFLAGS) \
		--entry=start --section-start=.text=0x7000 \
		--output=bootother.out bootother.o
	$(OBJCOPY) -S -O binary bootother.out $@

initcode: initcode.o
	$(LD) $(LDFLAGS) $(KERNEL_LDFLAGS) \
		--entry=start --section-start=.text=0x0 \
		--output=initcode.out initcode.o
	$(OBJCOPY) -S -O binary initcode.out $@

vectors.S: vectors.pl
	perl vectors.pl > $@

# default recipe for object files
%.o: %.c
	$(CC) $(CPPFLAGS) $(KERNEL_CPPFLAGS) $(CFLAGS) $(KERNEL_CFLAGS) -c -o $@ $<

%.o: %.S
	$(AS) $(CPPFLAGS) $(KERNEL_CPPFLAGS) $(ASFLAGS) $(KERNEL_ASFLAGS) -c $< -o $@


# default recipes for dependency files
%.d: %.c
	$(CC) $(CPPFLAGS) $(KERNEL_CPPFLAGS) $(CFLAGS) $(KERNEL_CFLAGS) \
		-M -MG $< -MF $@ -MT $@ -MT $(<:.c=.o)

%.d: %.S
	$(AS) $(CPPFLAGS) $(KERNEL_CPPFLAGS) $(CFLAGS) $(KERNEL_CFLAGS) \
		-M -MG $< -MF $@ -MT $@ -MT $(<:.S=.o)


# user programs
USER_PROGS := \
	cat\
	echo\
	forktest\
	grep\
	init\
	kill\
	ln\
	ls\
	mkdir\
	rm\
	sh\
	stressfs\
	tester\
	usertests\
	wc\
	zombie\
	clone\
	thread

USER_PROGS := $(USER_PROGS)

# user library files
USER_LIBS := \
	umalloc.o\
	ulib.o\
	usys.o\
	printf.o
	

USER_LIBS := $(USER_LIBS)

USER_OBJECTS = $(USER_PROGS:%=%.o) $(USER_LIBS)

USER_DEPS := $(USER_OBJECTS:.o=.d)

USER_CLEAN := bin $(USER_PROGS) $(USER_OBJECTS) $(USER_DEPS)

# add include dir to search path for headers
USER_CPPFLAGS += -I include

# do not search standard system paths for headers
USER_CPPFLAGS += -nostdinc

# disable PIC (position independent code)
USER_CFLAGS += -fno-pic

# do not use GCC builtin funtions (used to optimize common functions)
USER_CFLAGS += -fno-builtin

# disable some optimizations that assume one object will not alias an object
# of a different type
USER_CFLAGS += -fno-strict-aliasing

# disable stack smashing check
USER_CFLAGS += -fno-stack-protector

# generate code for 32-bit environment
USER_CFLAGS += -m32

# generate code for 32-bit environment
USER_ASFLAGS := $(USER_CFLAGS)

# FreeBSD ld wants ``elf_i386_fbsd''
USER_LDFLAGS += -m $(shell $(LD) -V | grep elf_i386 2>/dev/null)

# do not link with the host standard library files
USER_LDFLAGS += -nostdlib

# use simple contiguous section layout and do not use dynamic linking
USER_LDFLAGS += --omagic

# where program execution should begin
USER_LDFLAGS += --entry=main

# location in memory where the program will be loaded
USER_LDFLAGS += --section-start=.text=0x0

user/bin:
	mkdir -p user/bin

# user programs
bin/%: %.o $(USER_LIBS) | bin
	$(LD) $(LDFLAGS) $(USER_LDFLAGS) --output=$@ $< $(USER_LIBS)

# forktest has less library code linked in - needs to be small
# in order to be able to max out the proc table.
bin/forktest: forktest.o ulib.o usys.o umalloc.o | bin
	$(LD) $(LDFLAGS) $(USER_LDFLAGS) --output=$@ $^

# default recipe for object files
%.o: %.c
	$(CC) $(CPPFLAGS) $(USER_CPPFLAGS) $(CFLAGS) $(USER_CFLAGS) -c -o $@ $<

%.o: %.S
	$(CC) $(CPPFLAGS) $(USER_CPPFLAGS) $(ASFLAGS) $(USER_ASFLAGS) -c $< -o $@

# default recipes for dependency files
%.d: %.c
	$(CC) $(CPPFLAGS) $(USER_CPPFLAGS) $(CFLAGS) $(USER_CFLAGS) \
		-M -MG $< -MF $@ -MT $@ -MT $(<:.c=.o)

%.d: %.S
	$(CC) $(CPPFLAGS) $(USER_CPPFLAGS) $(ASFLAGS) $(USER_ASFLAGS) \
		-M -MG $< -MF $@ -MT $@ -MT $(<:.S=.o)


# dependency files
TOOLS_DEPS := mkfs.d

# all generated files
TOOLS_CLEAN := mkfs mkfs.o $(TOOLS_DEPS)

# flags
TOOLS_CPPFLAGS := -iquote include

# mkfs
mkfs: mkfs.o
	$(CC) $(LDFLAGS) $< -o $@

# build object files from c files
%.o: t%.c
	$(CC) -c $(CPPFLAGS) $(TOOLS_CPPFLAGS) $(CFLAGS) $(TOOLS_CLFAGS) -o $@ $<

# build dependency files form c files
%.d: %.c
	$(CC) $(CPPFLAGS) $(TOOLS_CPPFLAGS) $(CFLAGS) $(TOOLS_CFLAGS) \
	  -M -MG $< -MF $@ -MT $@ -MT $(<:.c=.o)


DEPS := $(KERNEL_DEPS) $(USER_DEPS) $(TOOLS_DEPS)
CLEAN := $(KERNEL_CLEAN) $(USER_CLEAN) $(TOOLS_CLEAN) \
	fs fs.img .gdbinit .bochsrc dist

.PHONY: clean distclean run depend qemu qemu-nox qemu-gdb qemu-nox-gdb bochs

# remove all generated files
clean:
	rm -rf $(CLEAN)

run: qemu

# run xv6 in qemu
qemu: fs.img xv6.img
	@echo Ctrl+a h for help
	$(QEMU) -serial mon:stdio $(QEMUOPTS)

# run xv6 in qemu without a display (serial only)
qemu-nox: fs.img xv6.img
	@echo Ctrl+a h for help
	$(QEMU) -nographic $(QEMUOPTS)

# run xv6 in qemu in debug mode
qemu-gdb: fs.img xv6.img .gdbinit
	@echo "Now run 'gdb' from another terminal." 1>&2
	@echo Ctrl+a h for help
	$(QEMU) -serial mon:stdio $(QEMUOPTS) -S $(QEMUGDB)

# run xv6 in qemu without a display (serial only) in debug mode
qemu-nox-gdb: fs.img xv6.img .gdbinit
	@echo "Now run 'gdb' from another terminal." 1>&2
	@echo Ctrl+a h for help
	$(QEMU) -nographic $(QEMUOPTS) -S $(QEMUGDB)

# run xv6 in bochs
bochs: fs.img xv6.img .bochsrc
	bochs -q

# generate dependency files
depend: $(DEPS)

dist: clean
	mkdir -p dist/xv6
	git describe --long > version
	cp -r version FILES Makefile \
		README .gitignore dist/xv6/
	cd dist && tar -czf xv6.tar.gz xv6
	mv dist/xv6.tar.gz .
	rm -rf dist


################################################################################
# Build Recipies
################################################################################

include $(DEPS)

%.asm: %.o
	$(OBJDUMP) -S $< > $@

%.sym: %.o
	$(OBJDUMP) -t $< | sed '1,/SYMBOL TABLE/d; s/ .* / /; /^$$/d' > $@

fs:
	mkdir -p fs

fs/%: bin/% | fs
	cp $< $@

fs/README: README | fs
	cp $< $@

USER_BINS := $(notdir $(USER_PROGS))
fs.img: mkfs fs/README $(addprefix fs/,$(USER_BINS))
	./mkfs fs.img fs

.gdbinit: dot-gdbinit
	sed "s/localhost:1234/localhost:$(GDBPORT)/" < $^ > $@

.bochsrc: dot-bochsrc
	cp dot-bochsrc .bochsrc

.DEFAULT:
	@echo "No rule to make target $@"
