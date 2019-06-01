	#
        # code to switch between user and kernel space.
        #
        # this code is mapped at the same virtual address
        # in user and kernel space so that it can switch
        # page tables.
	#
	# kernel.ld causes trampout to be aligned
        # to a page boundary.
        #
.globl usertrap
	.section trampoline
.globl trampout
trampout:
        # switch from kernel to user.
        # usertrapret() calls here.
	# a0: p->tf in user page table
        # a1: new value for satp, for user page table

        # switch to user page table
        csrw satp, a1

        # put the saved user a0 in sscratch, so we
        # can swap it with our a0 (p->tf) in the last step.
        ld t0, 112(a0)
        csrw sscratch, t0

        # restore all but a0 from p->tf
        ld ra, 40(a0)
        ld sp, 48(a0)
        ld gp, 56(a0)
        ld tp, 64(a0)
        ld t0, 72(a0)
        ld t1, 80(a0)
        ld t2, 88(a0)
        ld s0, 96(a0)
        ld s1, 104(a0)
        ld a1, 120(a0)
        ld a2, 128(a0)
        ld a3, 136(a0)
        ld a4, 144(a0)
        ld a5, 152(a0)
        ld a6, 160(a0)
        ld a7, 168(a0)
        ld s2, 176(a0)
        ld s3, 184(a0)
        ld s4, 192(a0)
        ld s5, 200(a0)
        ld s6, 208(a0)
        ld s7, 216(a0)
        ld s8, 224(a0)
        ld s9, 232(a0)
        ld s10, 240(a0)
        ld s11, 248(a0)
        ld t3, 256(a0)
        ld t4, 264(a0)
        ld t5, 272(a0)
        ld t6, 280(a0)

	# restore user a0, and save p->tf
        csrrw a0, sscratch, a0
        
        # return to user mode and user pc.
        # caller has set up sstatus and sepc.
        sret

.align 4
.globl trampin
trampin:    
	#
        # trap.c set stvec to point here, so
        # user interrupts and exceptions start here,
        # in supervisor mode, but with a
        # user page table.
        #
        # sscratch points to where the process's p->tf is
        # mapped into user space (TRAMPOLINE - 4096).
        #
        
	# swap a0 and sscratch
        # so that a0 is p->tf
        csrrw a0, sscratch, a0

        # save the user registers in p->tf
        sd ra, 40(a0)
        sd sp, 48(a0)
        sd gp, 56(a0)
        sd tp, 64(a0)
        sd t0, 72(a0)
        sd t1, 80(a0)
        sd t2, 88(a0)
        sd s0, 96(a0)
        sd s1, 104(a0)
        sd a1, 120(a0)
        sd a2, 128(a0)
        sd a3, 136(a0)
        sd a4, 144(a0)
        sd a5, 152(a0)
        sd a6, 160(a0)
        sd a7, 168(a0)
        sd s2, 176(a0)
        sd s3, 184(a0)
        sd s4, 192(a0)
        sd s5, 200(a0)
        sd s6, 208(a0)
        sd s7, 216(a0)
        sd s8, 224(a0)
        sd s9, 232(a0)
        sd s10, 240(a0)
        sd s11, 248(a0)
        sd t3, 256(a0)
        sd t4, 264(a0)
        sd t5, 272(a0)
        sd t6, 280(a0)

	# save the user a0 in p->tf->a0
        csrr t0, sscratch
        sd t0, 112(a0)

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
