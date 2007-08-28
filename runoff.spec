# types.h either
# param.h either
# defs.h either
# x86.h either
# asm.h either
# mmu.h either
# elf.h either
# mp.h either

even: bootasm.S  # mild preference
even: bootother.S  # mild preference
# bootmain.c either
even: main.c
# mp.c don't care at all
even: initcode.S
odd: init.c

# spinlock.h either
# spinlock.c either
even: proc.h  # mild preference
even: proc.c  # VERY important
# setjmp.S either
# kalloc.c either

# syscall.h either
# trapasm.S either
# traps.h either
even: trap.c  # important
# vectors.pl either
# syscall.c either
# sysproc.c either

# buf.h either
# dev.h either
# fcntl.h either
# stat.h either
# file.h either
# fs.h either
# fsvar.h either
# even: ide.c
# odd: bio.c
odd: fs.c   # VERY important
# file.c either
# exec.c either
# sysfile.c either

# even: pipe.c  # mild preference
# string.c either
even: console.c
odd: sh.c
