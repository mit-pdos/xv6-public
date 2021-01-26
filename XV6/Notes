bochs 2.2.6:
./configure --enable-smp --enable-disasm --enable-debugger --enable-all-optimizations --enable-4meg-pages --enable-global-pages --enable-pae --disable-reset-on-triple-fault
bochs CVS after 2.2.6:
./configure --enable-smp --enable-disasm --enable-debugger --enable-all-optimizations --enable-4meg-pages --enable-global-pages --enable-pae 

bootmain.c doesn't work right if the ELF sections aren't
sector-aligned. so you can't use ld -N. and the sections may also need
to be non-zero length, only really matters for tiny "kernels".

kernel loaded at 1 megabyte. stack same place that bootasm.S left it.

kinit() should find real mem size
  and rescue useable memory below 1 meg

no paging, no use of page table hardware, just segments

no user area: no magic kernel stack mapping
  so no copying of kernel stack during fork
  though there is a kernel stack page for each process

no kernel malloc(), just kalloc() for user core

user pointers aren't valid in the kernel

are interrupts turned on in the kernel? yes.

pass curproc explicitly, or implicit from cpu #?
  e.g. argument to newproc()?
  hmm, you need a global curproc[cpu] for trap() &c

no stack expansion

test running out of memory, process slots

we can't really use a separate stack segment, since stack addresses
need to work correctly as ordinary pointers. the same may be true of
data vs text. how can we have a gap between data and stack, so that
both can grow, without committing 4GB of physical memory? does this
mean we need paging?

perhaps have fixed-size stack, put it in the data segment?

oops, if kernel stack is in contiguous user phys mem, then moving
users' memory (e.g. to expand it) will wreck any pointers into the
kernel stack.

do we need to set fs and gs? so user processes can't abuse them?

setupsegs() may modify current segment table, is that legal?

trap() ought to lgdt on return, since currently only done in swtch()

protect hardware interrupt vectors from user INT instructions?

test out-of-fd cases for creating pipe.
test pipe reader closes then write
test two readers, two writers.
test children being inherited by grandparent &c

some sleep()s should be interruptible by kill()

locks
  init_lock
    sequences CPU startup
  proc_table_lock
    also protects next_pid
  per-fd lock *just* protects count read-modify-write
    also maybe freeness?
  memory allocator
  printf

in general, the table locks protect both free-ness and
  public variables of table elements
  in many cases you can use table elements w/o a lock
  e.g. if you are the process, or you are using an fd

lock order
  per-pipe lock
  proc_table_lock fd_table_lock kalloc_lock
  console_lock

do you have to be holding the mutex in order to call wakeup()? yes

device interrupts don't clear FL_IF
  so a recursive timer interrupt is possible

what does inode->busy mean?
  might be held across disk reads
  no-one is allowed to do anything to the inode
  protected by inode_table_lock
inode->count counts in-memory pointers to the struct
  prevents inode[] element from being re-used
  protected by inode_table_lock

blocks and inodes have ad-hoc sleep-locks
  provide a single mechanism?

kalloc() can return 0; do callers handle this right?

test: one process unlinks a file while another links to it
test: one process opens a file while another deletes it
test: deadlock d/.. vs ../d, two processes.
test: dup() shared fd->off
test: does echo foo > x truncate x?

sh: ioredirection incorrect now we have pipes
sh: chain of pipes won't work, also ugly that parent closes fdarray entries too
sh: dynamic memory allocation?
sh: should sh support ; () &
sh: stop stdin on ctrl-d (for cat > y)

really should have bdwrite() for file content
  and make some inode updates async
  so soft updates make sense

disk scheduling
echo foo > bar should truncate bar
  so O_CREATE should not truncate
  but O_TRUNC should

make it work on a real machine
release before acquire at end of sleep?
check 2nd disk (i.e. if not in .bochsrc)
