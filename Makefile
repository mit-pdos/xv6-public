OBJS = \
	console.o\
	file.o\
	ide.o\
	kalloc.o\
	lapic.o\
	ioapic.o\
	main.o\
	mp.o\
	picirq.o\
	pipe.o\
	proc.o\
	setjmp.o\
	spinlock.o\
	string.o\
	syscall.o\
	sysfile.o\
	sysproc.o\
	trapasm.o\
	trap.o\
	vectors.o\
	bio.o\
	fs.o\
	8253pit.o\

# Cross-compiling (e.g., on Mac OS X)
#TOOLPREFIX = i386-jos-elf-

# Using native tools (e.g., on X86 Linux)
TOOLPREFIX = 

CC = $(TOOLPREFIX)gcc
LD = $(TOOLPREFIX)ld
OBJCOPY = $(TOOLPREFIX)objcopy
OBJDUMP = $(TOOLPREFIX)objdump
CFLAGS = -fno-builtin -O2 -Wall -MD
AS = $(TOOLPREFIX)gas

xv6.img : bootblock kernel fs.img
	dd if=/dev/zero of=xv6.img count=10000
	dd if=bootblock of=xv6.img conv=notrunc
	dd if=kernel of=xv6.img seek=1 conv=notrunc

bootblock : bootasm.S bootmain.c
	$(CC) -O -nostdinc -I. -c bootmain.c
	$(CC) -nostdinc -I. -c bootasm.S
	$(LD) -N -e start -Ttext 0x7C00 -o bootblock.o bootasm.o bootmain.o
	$(OBJDUMP) -S bootblock.o > bootblock.asm
	$(OBJCOPY) -S -O binary bootblock.o bootblock
	./sign.pl bootblock

kernel : $(OBJS) bootother.S _init
	$(CC) -nostdinc -I. -c bootother.S
	$(LD) -N -e start -Ttext 0x7000 -o bootother.out bootother.o
	$(OBJCOPY) -S -O binary bootother.out bootother
	$(OBJDUMP) -S bootother.o > bootother.asm
	$(LD) -Ttext 0x100000 -e main0 -o kernel $(OBJS) -b binary bootother _init
	$(OBJDUMP) -S kernel > kernel.asm

tags: $(OBJS) bootother.S _init
	etags *.S *.c

vectors.S : vectors.pl
	perl vectors.pl > vectors.S

ULIB = ulib.o usys.o printf.o umalloc.o

usertests : usertests.o $(ULIB)
	$(LD) -N -e main -Ttext 0 -o usertests usertests.o $(ULIB)
	$(OBJDUMP) -S usertests > usertests.asm

_echo : echo.o $(ULIB)
	$(LD) -N -e main -Ttext 0 -o _echo echo.o $(ULIB)
	$(OBJDUMP) -S _echo > echo.asm

_cat : cat.o $(ULIB)
	$(LD) -N -e main -Ttext 0 -o _cat cat.o $(ULIB)
	$(OBJDUMP) -S _cat > cat.asm

_init : init.o $(ULIB)
	$(LD) -N -e main -Ttext 0 -o _init init.o $(ULIB)
	$(OBJDUMP) -S _init > init.asm

_sh : sh.o $(ULIB)
	$(LD) -N -e main -Ttext 0 -o _sh sh.o $(ULIB)
	$(OBJDUMP) -S _sh > sh.asm

_ls : ls.o $(ULIB)
	$(LD) -N -e main -Ttext 0 -o _ls ls.o $(ULIB)
	$(OBJDUMP) -S _ls > ls.asm

_mkdir : mkdir.o $(ULIB)
	$(LD) -N -e main -Ttext 0 -o _mkdir mkdir.o $(ULIB)
	$(OBJDUMP) -S _mkdir > mkdir.asm

_rm : rm.o $(ULIB)
	$(LD) -N -e main -Ttext 0 -o _rm rm.o $(ULIB)
	$(OBJDUMP) -S _rm > rm.asm

mkfs : mkfs.c fs.h
	cc -o mkfs mkfs.c

fs.img : mkfs usertests _echo _cat README _init _sh _ls _mkdir _rm 
	./mkfs fs.img usertests _echo _cat README _init _sh _ls _mkdir _rm

-include *.d

clean : 
	/bin/rm -f rm
	rm -f *.ps *.tex *.dvi *.idx *.aux *.log *.ind *.ilg \
	*.o *.d *.asm vectors.S parport.out \
	bootblock kernel xv6.img usertests \
	fs.img mkfs echo init

# make a printout
PRINT =	\
	runoff.list \
	README\
	types.h param.h defs.h x86.h asm.h elf.h mmu.h spinlock.h\
	bootasm.S bootother.S main.c init.c spinlock.c\
	proc.h proc.c setjmp.S kalloc.c\
	syscall.h trapasm.S traps.h trap.c vectors.pl syscall.c sysproc.c\
	buf.h dev.h fcntl.h stat.h file.h fs.h fsvar.h file.c fs.c bio.c ide.c sysfile.c\
	pipe.c\
	mp.h ioapic.h mp.c lapic.c ioapic.c picirq.c\
	console.c\
	string.c\

xv6.pdf : $(PRINT)
	./runoff

print : xv6.pdf

# run in emulators

bochs : fs.img xv6.img
	if [ ! -e .bochsrc ]; then ln -s dot-bochsrc .bochsrc; fi
	bochs -q

qemu : fs.img xv6.img
	qemu -parallel stdio -hdb fs.img xv6.img

