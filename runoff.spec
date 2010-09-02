sheet1: left

# "left" and "right" specify which page of a two-page spread a file
# must start on.  "left" means that a file must start on the first of
# the two page.  "right" means it must start on the second of the two
# pages.  The file may start in either column.
#
# "even" and "odd" specify which column a file must start on.  "even"
# means it must start in the left of the two columns.  "odd" means it
# must start in the right of the two columns.

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
right: proc.h
odd: proc.h

# goal is to have two action-packed 2-page spreads,
# one with
#     userinit growproc fork exit wait
# and another with
#     scheduler sched yield forkret sleep wakeup1 wakeup
left: proc.c   # VERY important
odd: proc.c   # VERY important

# A few more action packed spreads
# page table creation and process loading
#     walkpgdir mappages setupkvm vmenable switch[ku]vm inituvm loaduvm
# process memory management
#     allocuvm deallocuvm freevm
right: vm.c
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
left: ide.c
# odd: bio.c
odd: fs.c   # VERY important
# file.c either
# exec.c either
# sysfile.c either

# even: pipe.c  # mild preference
# string.c either
left: kbd.h
even: console.c
odd: sh.c
