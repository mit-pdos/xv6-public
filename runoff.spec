sheet1: left

# "left" and "right" specify which page of a two-page spread a file
# must start on.  "left" means that a file must start on the first of
# the two pages.  "right" means it must start on the second of the two
# pages.  The file may start in either column.
#
# "even" and "odd" specify which column a file must start on.  "even"
# means it must start in the left of the two columns (00).  "odd" means it
# must start in the right of the two columns (50).
#
# You'd think these would be the other way around.

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
even: bootmain.c  # mild preference
even: main.c
# mp.c don't care at all
# even: initcode.S
# odd: init.c

# spinlock.h either
left: spinlock.h  # mild preference
even: spinlock.h  # mild preference

# This gets struct proc and allocproc on the same spread
left: proc.h
even: proc.h

# goal is to have two action-packed 2-page spreads,
# one with
#     userinit growproc fork exit wait
# and another with
#     scheduler sched yield forkret sleep wakeup1 wakeup
right: proc.c   # VERY important
even: proc.c   # VERY important

# A few more action packed spreads
# page table creation and process loading
#     walkpgdir mappages setupkvm vmenable switch[ku]vm inituvm loaduvm
# process memory management
#     allocuvm deallocuvm freevm
left: vm.c
odd: vm.c

# kalloc.c either

# syscall.h either
# trapasm.S either
# traps.h either
# even: trap.c
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
# left: ide.c # mild preference
even: ide.c
# odd: bio.c

# with fs.c starting on 2nd column of a left page, we get these 2-page spreads:
#	ialloc iupdate iget idup ilock iunlock iput iunlockput
#	bmap itrunc stati readi writei
#	namecmp dirlookup dirlink skipelem namex namei
#	fielinit filealloc filedup fileclose filestat fileread filewrite
# starting on 2nd column of a right page is not terrible either
odd: fs.c   # VERY important
left: fs.c  # mild preference
# file.c either
# exec.c either
# sysfile.c either

# even: pipe.c  # mild preference
# string.c either
# left: kbd.h  # mild preference
even: kbd.h
even: console.c
odd: sh.c
