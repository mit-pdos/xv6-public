OBJS = main.o console.o string.o kalloc.o proc.o trapasm.o trap.o vectors.o \
       syscall.o ide.o picirq.o mp.o spinlock.o fd.o pipe.o swtch.o

# Cross-compiling (e.g., on Mac OS X)
TOOLPREFIX = i386-jos-elf-

# Using native tools (e.g., on X86 Linux)
# TOOLPREFIX = 

CC = $(TOOLPREFIX)gcc
LD = $(TOOLPREFIX)ld
OBJCOPY = $(TOOLPREFIX)objcopy
OBJDUMP = $(TOOLPREFIX)objdump
CFLAGS = -nostdinc -I. -O2 -Wall -MD
AS = $(TOOLPREFIX)gas

xv6.img : bootblock kernel
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

kernel : $(OBJS) bootother.S user1 usertests userfs
	$(CC) -nostdinc -I. -c bootother.S
	$(LD) -N -e start -Ttext 0x7000 -o bootother.out bootother.o
	$(OBJCOPY) -S -O binary bootother.out bootother
	$(OBJDUMP) -S bootother.o > bootother.asm
	$(LD) -Ttext 0x100000 -e main -o kernel $(OBJS) -b binary bootother user1 usertests userfs
	$(OBJDUMP) -S kernel > kernel.asm

vectors.S : vectors.pl
	perl vectors.pl > vectors.S

ULIB = ulib.o usys.o

user1 : user1.c $(ULIB)
	$(CC) -nostdinc -I. -c user1.c
	$(LD) -N -e main -Ttext 0 -o user1 user1.o $(ULIB)
	$(OBJDUMP) -S user1 > user1.asm

usertests : usertests.c $(ULIB)
	$(CC) -nostdinc -I. -c usertests.c
	$(LD) -N -e main -Ttext 0 -o usertests usertests.o $(ULIB)
	$(OBJDUMP) -S usertests > usertests.asm

userfs : userfs.c $(ULIB)
	$(CC) -nostdinc -I. -c userfs.c
	$(LD) -N -e main -Ttext 0 -o userfs userfs.o $(ULIB)
	$(OBJDUMP) -S userfs > userfs.asm

ulib.o : ulib.c
	$(CC) -nostdinc -I. -c ulib.c

-include *.d

clean : 
	rm -f *.o bootblock kernel kernel.asm xv6.img *.d user1
