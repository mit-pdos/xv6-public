OBJS = main.o console.o string.o kalloc.o proc.o trapasm.o

CC = i386-jos-elf-gcc
LD = i386-jos-elf-ld
OBJCOPY = i386-jos-elf-objcopy
OBJDUMP = i386-jos-elf-objdump

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

kernel : $(OBJS)
	$(LD) -Ttext 0x100000 -e main -o kernel $(OBJS)
	$(OBJDUMP) -S kernel > kernel.asm

%.o: %.c
	$(CC) -nostdinc -I. -O -c -o $@ $<

clean : 
	rm -f bootmain.o bootasm.o bootblock.o bootblock
	rm -f kernel main.o kernel.asm xv6.img
