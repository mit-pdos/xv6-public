	#
        # code to switch between user and kernel space.
        #
        # this code is mapped at the same virtual address
        # in user and kernel space so that it can switch
        # page tables.
	#
	# kernel.ld causes trampstart to be aligned
        # to a page boundary.
        #
.globl usertrap
	.section trampoline
.globl trampstart
trampstart:
        # switch from kernel to user.
	# a0: p->tf in user page table
        # a1: new value for satp, for user page table

        # switch to user page table
        csrw satp, a1

        # put the saved user a0 in sscratch, so we
        # can swap it with our a0 (p->tf) in the last step.
        ld t0, 80(a0)
        csrw sscratch, t0

        # restore all but a0 from p->tf
        ld ra, 32(a0)
        ld sp, 40(a0)
        ld gp, 48(a0)
        ld tp, 56(a0)
        ld t0, 64(a0)
        ld t1, 72(a0)
        ld t2, 80(a0)
        ld a1, 96(a0)
        ld a2, 104(a0)
        ld a3, 112(a0)
        ld a4, 120(a0)
        ld a5, 128(a0)
        ld a6, 136(a0)
        ld a7, 144(a0)
        ld t3, 152(a0)
        ld t4, 160(a0)
        ld t5, 168(a0)
        ld t6, 176(a0)

	# restore user a0, and save p->tf
        csrrw a0, sscratch, a0
        
        # return to user mode and user pc.
        # caller has set up sstatus and sepc.
        sret

	#
        # trap.c set stvec to point here, so
        # interrupts and exceptions start here,
        # in supervisor mode, but with a
        # user page table.
        #
        # sscratch points to where the process's p->tf is
        # mapped into user space (TRAMPOLINE - 4096).
        #
.align 4
.globl trampvec
trampvec:    
	# swap a0 and sscratch
        # so that a0 is p->tf
        csrrw a0, sscratch, a0

        # save the user registers in p->tf
        sd ra, 32(a0)
        sd sp, 40(a0)
        sd gp, 48(a0)
        sd tp, 56(a0)
        sd t0, 64(a0)
        sd t1, 72(a0)
        sd t2, 80(a0)
        sd a1, 96(a0)
        sd a2, 104(a0)
        sd a3, 112(a0)
        sd a4, 120(a0)
        sd a5, 128(a0)
        sd a6, 136(a0)
        sd a7, 144(a0)
        sd t3, 152(a0)
        sd t4, 160(a0)
        sd t5, 168(a0)
        sd t6, 176(a0)

	# save the user a0 in p->tf->a0
        csrr t0, sscratch
        sd t0, 80(a0)

        # restore kernel stack pointer from p->tf->kernel_sp
        ld sp, 8(a0)

        # remember the address of usertrap(), p->tf->kernel_trap
        ld t0, 16(a0)

        # restore kernel page table from p->tf->kernel_satp
        ld t1, 0(a0)
        csrw satp, t1

        # a0 is no longer valid, since the kernel page
        # table does not specially map p->td.

        # jump to usertrap(), which does not return
        jr t0
