OBJS = main.o console.o string.o kalloc.o proc.o trapasm.o trap.o vectors.o \
       syscall.o ide.o picirq.o mp.o spinlock.o

CC = i386-jos-elf-gcc
LD = i386-jos-elf-ld
OBJCOPY = i386-jos-elf-objcopy
OBJDUMP = i386-jos-elf-objdump
CFLAGS = -nostdinc -I. -O -Wall -MD

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

kernel : $(OBJS) bootother.S user1
	$(CC) -nostdinc -I. -c bootother.S
	$(LD) -N -e start -Ttext 0x7000 -o bootother.out bootother.o
	$(OBJCOPY) -S -O binary bootother.out bootother
	$(OBJDUMP) -S bootother.o > bootother.asm
	$(LD) -Ttext 0x100000 -e main -o kernel $(OBJS) -b binary bootother user1
	$(OBJDUMP) -S kernel > kernel.asm

vectors.S : vectors.pl
	perl vectors.pl > vectors.S

user1 : user1.c
	$(CC) -nostdinc -I. -c user1.c
	$(LD) -N -e main -Ttext 0 -o user1 user1.o
	$(OBJDUMP) -S user1 > user1.asm

-include *.d

clean : 
	rm -f *.o bootblock kernel kernel.asm xv6.img *.d user1
