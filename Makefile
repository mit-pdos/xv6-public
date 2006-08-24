OBJS = \
	console.o\
	fd.o\
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
	trapasm.o\
	trap.o\
	vectors.o\
	bio.o\
	fs.o\

# Cross-compiling (e.g., on Mac OS X)
TOOLPREFIX = i386-jos-elf-

# Using native tools (e.g., on X86 Linux)
# TOOLPREFIX = 

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

kernel : $(OBJS) bootother.S init
	$(CC) -nostdinc -I. -c bootother.S
	$(LD) -N -e start -Ttext 0x7000 -o bootother.out bootother.o
	$(OBJCOPY) -S -O binary bootother.out bootother
	$(OBJDUMP) -S bootother.o > bootother.asm
	$(LD) -Ttext 0x100000 -e main0 -o kernel $(OBJS) -b binary bootother init
	$(OBJDUMP) -S kernel > kernel.asm

vectors.S : vectors.pl
	perl vectors.pl > vectors.S

ULIB = ulib.o usys.o printf.o umalloc.o

user1 : user1.o $(ULIB)
	$(LD) -N -e main -Ttext 0 -o user1 user1.o $(ULIB)
	$(OBJDUMP) -S user1 > user1.asm

usertests : usertests.o $(ULIB)
	$(LD) -N -e main -Ttext 0 -o usertests usertests.o $(ULIB)
	$(OBJDUMP) -S usertests > usertests.asm

fstests : fstests.o $(ULIB)
	$(LD) -N -e main -Ttext 0 -o fstests fstests.o $(ULIB)
	$(OBJDUMP) -S fstests > fstests.asm

echo : echo.o $(ULIB)
	$(LD) -N -e main -Ttext 0 -o echo echo.o $(ULIB)
	$(OBJDUMP) -S echo > echo.asm

cat : cat.o $(ULIB)
	$(LD) -N -e main -Ttext 0 -o cat cat.o $(ULIB)
	$(OBJDUMP) -S cat > cat.asm

userfs : userfs.o $(ULIB)
	$(LD) -N -e main -Ttext 0 -o userfs userfs.o $(ULIB)
	$(OBJDUMP) -S userfs > userfs.asm

init : init.o $(ULIB)
	$(LD) -N -e main -Ttext 0 -o init init.o $(ULIB)
	$(OBJDUMP) -S init > init.asm

sh : sh.o $(ULIB)
	$(LD) -N -e main -Ttext 0 -o sh sh.o $(ULIB)
	$(OBJDUMP) -S sh > sh.asm

ls : ls.o $(ULIB)
	$(LD) -N -e main -Ttext 0 -o ls ls.o $(ULIB)
	$(OBJDUMP) -S ls > ls.asm

mkdir : mkdir.o $(ULIB)
	$(LD) -N -e main -Ttext 0 -o mkdir mkdir.o $(ULIB)
	$(OBJDUMP) -S mkdir > mkdir.asm

rm : rm.o $(ULIB)
	$(LD) -N -e main -Ttext 0 -o rm rm.o $(ULIB)
	$(OBJDUMP) -S rm > rm.asm

mkfs : mkfs.c fs.h
	cc -o mkfs mkfs.c

fs.img : mkfs userfs usertests echo cat readme init sh ls mkdir rm fstests
	./mkfs fs.img userfs usertests echo cat readme init sh ls mkdir rm fstests

-include *.d

clean : 
	rm -f *.o *.d *.asm vectors.S parport.out \
		bootblock kernel xv6.img user1 userfs usertests \
		fs.img mkfs echo init fstests
