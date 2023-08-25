KERNOBJS = \
	bio.o console.o exec.o file.o fs.o ide.o ioapic.o kalloc.o kbd.o lapic.o \
  log.o main.o mp.o pipe.o proc.o sleeplock.o spinlock.o string.o swtch.o \
  syscall.o sysfile.o sysproc.o trapasm.o trap.o uart.o vectors.o vm.o \
#

UNAME_S := $(shell uname -s)
UNAME_P := $(shell uname -p)
ifeq ($(UNAME_S),Linux)
  ifeq ($(UNAME_P),aarch64)
	TOOLPREFIX = x86_64-linux-gnu
  else
  	# no prefix for Linux or WSL2
 	 TOOLPREFIX = 
  endif
else ifeq ($(UNAME_S),Darwin)
  # need homebrew: $ brew install qemu x86_64-elf-gcc
  TOOLPREFIX = x86_64-elf-
endif

# If the makefile can't find QEMU, specify its path here
QEMU = qemu-system-x86_64

CC = $(TOOLPREFIX)gcc
AS = $(TOOLPREFIX)as
LD = $(TOOLPREFIX)ld
OBJCOPY = $(TOOLPREFIX)objcopy
OBJDUMP = $(TOOLPREFIX)objdump
OPT ?= -O0
XFLAGS = -m64 -DX64 -mcmodel=large -mtls-direct-seg-refs -mno-red-zone

CFLAGS = -fno-pic -static -fno-builtin -fno-strict-aliasing -Wall -MD -ggdb -fno-omit-frame-pointer
CFLAGS += -ffreestanding -fno-common -nostdlib $(XFLAGS) $(OPT)
CFLAGS += $(shell $(CC) -fno-stack-protector -E -x c /dev/null >/dev/null 2>&1 && echo -fno-stack-protector)

ASFLAGS = -gdwarf-2 -Wa,-divide $(XFLAGS)
LDFLAGS = -m elf_x86_64 -nostdlib


xv6.img: bootblock kernel fs.img
	dd if=/dev/zero of=xv6.img count=10000
	dd if=bootblock of=xv6.img conv=notrunc
	dd if=kernel of=xv6.img seek=1 conv=notrunc

xv6memfs.img: bootblock kernelmemfs
	dd if=/dev/zero of=xv6memfs.img count=10000
	dd if=bootblock of=xv6memfs.img conv=notrunc
	dd if=kernelmemfs of=xv6memfs.img seek=1 conv=notrunc

bootblock: bootasm.S bootmain.c
	$(CC) -fno-builtin -fno-pic -m32 -nostdinc -c bootasm.S
	$(CC) -fno-builtin -fno-pic -m32 -O -nostdinc -c bootmain.c
	$(LD) -m elf_i386 -nostdlib -n -N -e start -Ttext 0x7C00 -o bootblocktmp.o bootasm.o bootmain.o
	$(OBJDUMP) -S bootblocktmp.o > bootblock.asm
	$(OBJCOPY) -S -O binary -j .text bootblocktmp.o bootblock
	./sign.pl bootblock

entryother: entryother.S
	$(CC) $(CFLAGS) -fno-pic -nostdinc -I. -c entryother.S
	$(LD) $(LDFLAGS) -n -N -e start -Ttext 0x7000 -o entryothertmp.o entryother.o
	$(OBJDUMP) -S entryothertmp.o > entryother.asm
	$(OBJCOPY) -S -O binary -j .text entryothertmp.o entryother

initcode: initcode.S
	$(CC) $(CFLAGS) -fno-pic -nostdinc -I. -c initcode.S
	$(LD) $(LDFLAGS) -n -N -e start -Ttext 0x1000 -o initcodetmp.o initcode.o
	$(OBJDUMP) -S initcodetmp.o > initcode.asm
	$(OBJCOPY) -S -O binary -j .text initcodetmp.o initcode

kernel: $(KERNOBJS) entry.o entryother initcode kernel.ld
	$(LD) $(LDFLAGS) -n -N -T kernel.ld -o kernel entry.o $(KERNOBJS) -b binary initcode entryother
	$(OBJDUMP) -S kernel > kernel.asm
	$(OBJDUMP) -t kernel | sed '1,/SYMBOL TABLE/d; s/ .* / /; /^$$/d' | sort > kernel.sym

depend: .depend

.depend: $(wildcard *.c)
	rm -f "$@"
	$(CC) -MM $^ > "$@"

include .depend
# kernelmemfs is a copy of kernel that maintains the
# disk image in memory instead of writing to a disk.
# This is not so useful for testing persistent storage or
# exploring disk buffering implementations, but it is
# great for testing the kernel on real hardware without
# needing a scratch disk.
MEMFSOBJS = $(filter-out ide.o,$(KERNOBJS)) memide.o
kernelmemfs: $(MEMFSOBJS) entry.o entryother initcode kernel.ld fs.img
	$(LD) $(LDFLAGS) -T kernel.ld -o kernelmemfs entry.o  $(MEMFSOBJS) -b binary initcode entryother fs.img
	$(OBJDUMP) -S kernelmemfs > kernelmemfs.asm
	$(OBJDUMP) -t kernelmemfs | sed '1,/SYMBOL TABLE/d; s/ .* / /; /^$$/d' | sort > kernelmemfs.sym

vectors.S: vectors.pl
	perl vectors.pl > vectors.S

ULIB = ulib.o usys.o printf.o umalloc.o

_%: %.o $(ULIB) user.ld
	$(LD) $(LDFLAGS) -n -N -T user.ld -e main -Ttext 0x1000 -o $@ $< $(ULIB)
	$(OBJDUMP) -S $@ > $*.asm
	$(OBJDUMP) -t $@ | sed '1,/SYMBOL TABLE/d; s/ .* / /; /^$$/d' | sort > $*.sym

_forktest: forktest.o $(ULIB) user.ld
	# forktest has less library code linked in - needs to be small
	# in order to be able to max out the proc table.
	$(LD) $(LDFLAGS) -n -N -T user.ld -e main -Ttext 0x1000 -o $@ $< ulib.o usys.o
	$(OBJDUMP) -S _forktest > forktest.asm

mkfs: mkfs.c fs.h
	gcc -Werror -Wall -o mkfs mkfs.c

# Prevent deletion of intermediate files, e.g. cat.o, after first build, so
# that disk image changes after first build are persistent until clean.  More
# details:
# http://www.gnu.org/software/make/manual/html_node/Chained-Rules.html
.PRECIOUS: %.o

UPROGS= \
	_cat _echo _forktest _grep _init _kill _ln _ls _mkdir \
	_rm _sh _stressfs _usertests _wc _zombie \
#

fs.img: mkfs README $(UPROGS)
	./mkfs fs.img README $(UPROGS)

clean:
	rm -f *.tex *.dvi *.idx *.aux *.log *.ind *.ilg \
	*.o *.d *.asm *.sym vectors.S bootblock entryother \
	initcode initcode.out kernel xv6.img fs.img kernelmemfs mkfs \
	.gdbinit \
	_*

# make a printout
FILES = $(shell grep -v '^\#' runoff.list)
PRINT = runoff.list runoff.spec README toc.hdr toc.ftr $(FILES)

xv6.pdf: $(PRINT)
	./runoff
	ls -l xv6.pdf

xv6_marked.pdf: xv6.pdf pdfmark
	gs -o xv6_marked.pdf -sDEVICE=pdfwrite -dPDFSETTINGS=/prepress xv6.pdf pdfmark


print: xv6.pdf

# run in emulators

# try to generate a unique GDB port
GDBPORT = $(shell expr `id -u` % 5000 + 25000)
# QEMU's gdb stub command line changed in 0.11
QEMUGDB = $(shell if $(QEMU) -help | grep -q '^-gdb'; \
	then echo "-gdb tcp::$(GDBPORT)"; \
	else echo "-s -p $(GDBPORT)"; fi)
ifndef CPUS
CPUS := 2
endif
QEMUOPTS = -nic none -hda xv6.img -hdb fs.img -smp $(CPUS) -m 512 $(QEMUEXTRA)

qemu: fs.img xv6.img
	$(QEMU) -serial mon:stdio $(QEMUOPTS)

qemu-memfs: xv6memfs.img
	$(QEMU) -drive file=xv6memfs.img,index=0,media=disk,format=raw -smp $(CPUS) -m 256

qemu-nox: fs.img xv6.img
	$(QEMU) -nographic $(QEMUOPTS) -action reboot=shutdown

.gdbinit: .gdbinit.tmpl
	sed "s/localhost:1234/localhost:$(GDBPORT)/" < $^ > $@

qemu-gdb: fs.img xv6.img .gdbinit
	@echo "*** Now run 'gdb'." 1>&2
	$(QEMU) -serial mon:stdio $(QEMUOPTS) -S $(QEMUGDB)

qemu-nox-gdb: fs.img xv6.img .gdbinit
	@echo "*** Now run 'gdb'." 1>&2
	$(QEMU) -nographic $(QEMUOPTS) -S $(QEMUGDB)

# CUT HERE
# prepare dist for students
# after running make dist, probably want to
# rename it to rev0 or rev1 or so on and then
# check in that version.

EXTRA=\
	mkfs.c ulib.c user.h cat.c echo.c forktest.c grep.c kill.c\
	ln.c ls.c mkdir.c rm.c stressfs.c usertests.c wc.c zombie.c\
	printf.c umalloc.c\
	README *.pl toc.* runoff runoff1 runoff.list\
	.gdbinit.tmpl

dist:
	rm -rf dist
	mkdir dist
	for i in $(FILES); \
	do \
		grep -v PAGEBREAK $$i >dist/$$i; \
	done
	sed '/CUT HERE/,$$d' Makefile >dist/Makefile
	echo >dist/runoff.spec
	cp $(EXTRA) dist

dist-test:
	rm -rf dist
	make dist
	rm -rf dist-test
	mkdir dist-test
	cp dist/* dist-test
	cd dist-test; $(MAKE) print
	cd dist-test; $(MAKE) qemu

# update this rule (change rev#) when it is time to
# make a new revision.
tar:
	rm -rf /tmp/xv6
	mkdir -p /tmp/xv6
	cp dist/* dist/.gdbinit.tmpl /tmp/xv6
	(cd /tmp; tar cf - xv6) | gzip >xv6-rev10.tar.gz  # the next one will be 10 (9/17)

.PHONY: dist-test dist
