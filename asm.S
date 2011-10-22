.code64
.text

.globl inb
inb:
	movq %rdi, %rdx
	xorq %rax, %rax
	inb %dx, %al
	ret

.globl outb
outb:
	movq %rdi, %rdx
	movq %rsi, %rax
	outb %al, %dx
	ret

.globl hlt
hlt:
	hlt
	jmp hlt

.globl lidt
lidt:
	# Create pseudo-descriptor on stack:
	# 2 bytes length-1 followed by 8 bytes base address.
	subq $16, %rsp
	subq $1, %rsi
	movw %si, 0(%rsp)
	movq %rdi, 2(%rsp)
	lidt 0(%rsp)
	addq $16, %rsp
	ret

.globl lgdt
lgdt:
	# Create pseudo-descriptor on stack:
	# 2 bytes length-1 followed by 8 bytes base address.
	subq $16, %rsp
	subq $1, %rsi
	movw %si, 0(%rsp)
	movq %rdi, 2(%rsp)
	lgdt 0(%rsp)
	addq $16, %rsp
	ret

.globl ltr
ltr:
	ltr %di
	ret

// Delay for the given number of microseconds.
// Assume 20 atomic exchanges = 1 microsecond.
// Close enough for now.
.globl microdelay
microdelay:
	movq %rdi, %rcx
	imul $20, %rcx
	pushq $0
1:
	lock xchgq %rax, (%rsp)
	loop 1b
	popq %rax
	ret

.globl readeflags
readeflags:
	pushfq
	popq %rax
	ret
