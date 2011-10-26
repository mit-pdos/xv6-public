#include "mmu.h"

#define NOEC pushq $0
#define EC

#define _TRAP(x, code) \
	.globl trap ## x; \
	.align 8; \
	trap ## x: \
                code; \
	        pushq $x; \
		jmp trapcommon; \
	.data; \
	.quad trap ## x; \
	.previous
        
#define TRAP(x) _TRAP(x, NOEC)
#define TRAPCODE(x) _TRAP(x, EC)

.code64
.globl sysentry
.align 8
sysentry:
        jmp sysentry

.globl trappush
.align 8
trappush:
	pushq $0  // error code
trapcommon:
        pushq   %rdi
        pushq   %rsi
        pushq   %rdx
        pushq   %rcx
        pushq   %rax
        pushq   %r8
        pushq   %r9
        pushq   %r10
        pushq   %r11
        pushq   %rbx
        pushq   %rbp
        pushq   %r12
        pushq   %r13
        pushq   %r14
        pushq   %r15
        movw    %ds, %ax
        pushw   %ax
        subq    $0xe, %rsp
        
        # Set up data and per-cpu segments.
        movw    $KDSEG, %ax
        movw    %ax, %ds
        movw    %ax, %es
        // XXX(sbw) we should do something with fs, gs, gs.base

        movq	%rsp, %rdi  // first argument to trap
        call	trap
        // Fall through to trapret
        
.globl trapret
.align 8
trapret:
        popw %ax
        movw %ax, %ds
        movw %ax, %es
        // XXX(sbw) we should do something with fs, gs, gs.base
        addq    $0xe, %rsp  // padding
        
        popq    %r15
        popq    %r14
        popq    %r13
        popq    %r12
        popq    %rbp
        popq    %rbx
        popq    %r11
        popq    %r10
        popq    %r9
        popq    %r8
        popq    %rax
        popq    %rcx
        popq    %rdx
        popq    %rsi
        popq    %rdi
        addq    $0x10, %rsp  // trapno, err
        iretq
        
.data
.align 8
.globl trapentry
trapentry:

.text
	// See section 6.15 of Vol 3.
	TRAP(0)  // divide error
	TRAP(1)  // debug exception
	TRAP(2)  // NMI
	TRAP(3)  // breakpoint
	TRAP(4)  // overflow
	TRAP(5)  // bound
	TRAP(6)  // invalid opcode
	TRAP(7)  // device not available
	TRAPCODE(8)  // double fault - has error code
	TRAP(9)  // coprocessor overrun
	TRAPCODE(10)  // invalid TSS
	TRAPCODE(11)  // segment not present
	TRAPCODE(12)  // stack fault
	TRAPCODE(13)  // general protection fault
	TRAPCODE(14)  // page fault
	TRAP(15)  // not defined
	TRAP(16)  // fpu error
	TRAPCODE(17)  // alignment check
	TRAP(18)  // machine check
	TRAP(19)  // simd
	# Edit .+1,|seq 20 255 | sed 's/.*/	TRAP(&)/'
	TRAP(20)
	TRAP(21)
	TRAP(22)
	TRAP(23)
	TRAP(24)
	TRAP(25)
	TRAP(26)
	TRAP(27)
	TRAP(28)
	TRAP(29)
	TRAP(30)
	TRAP(31)
	TRAP(32)
	TRAP(33)
	TRAP(34)
	TRAP(35)
	TRAP(36)
	TRAP(37)
	TRAP(38)
	TRAP(39)
	TRAP(40)
	TRAP(41)
	TRAP(42)
	TRAP(43)
	TRAP(44)
	TRAP(45)
	TRAP(46)
	TRAP(47)
	TRAP(48)
	TRAP(49)
	TRAP(50)
	TRAP(51)
	TRAP(52)
	TRAP(53)
	TRAP(54)
	TRAP(55)
	TRAP(56)
	TRAP(57)
	TRAP(58)
	TRAP(59)
	TRAP(60)
	TRAP(61)
	TRAP(62)
	TRAP(63)
	TRAP(64)
	TRAP(65)
	TRAP(66)
	TRAP(67)
	TRAP(68)
	TRAP(69)
	TRAP(70)
	TRAP(71)
	TRAP(72)
	TRAP(73)
	TRAP(74)
	TRAP(75)
	TRAP(76)
	TRAP(77)
	TRAP(78)
	TRAP(79)
	TRAP(80)
	TRAP(81)
	TRAP(82)
	TRAP(83)
	TRAP(84)
	TRAP(85)
	TRAP(86)
	TRAP(87)
	TRAP(88)
	TRAP(89)
	TRAP(90)
	TRAP(91)
	TRAP(92)
	TRAP(93)
	TRAP(94)
	TRAP(95)
	TRAP(96)
	TRAP(97)
	TRAP(98)
	TRAP(99)
	TRAP(100)
	TRAP(101)
	TRAP(102)
	TRAP(103)
	TRAP(104)
	TRAP(105)
	TRAP(106)
	TRAP(107)
	TRAP(108)
	TRAP(109)
	TRAP(110)
	TRAP(111)
	TRAP(112)
	TRAP(113)
	TRAP(114)
	TRAP(115)
	TRAP(116)
	TRAP(117)
	TRAP(118)
	TRAP(119)
	TRAP(120)
	TRAP(121)
	TRAP(122)
	TRAP(123)
	TRAP(124)
	TRAP(125)
	TRAP(126)
	TRAP(127)
	TRAP(128)
	TRAP(129)
	TRAP(130)
	TRAP(131)
	TRAP(132)
	TRAP(133)
	TRAP(134)
	TRAP(135)
	TRAP(136)
	TRAP(137)
	TRAP(138)
	TRAP(139)
	TRAP(140)
	TRAP(141)
	TRAP(142)
	TRAP(143)
	TRAP(144)
	TRAP(145)
	TRAP(146)
	TRAP(147)
	TRAP(148)
	TRAP(149)
	TRAP(150)
	TRAP(151)
	TRAP(152)
	TRAP(153)
	TRAP(154)
	TRAP(155)
	TRAP(156)
	TRAP(157)
	TRAP(158)
	TRAP(159)
	TRAP(160)
	TRAP(161)
	TRAP(162)
	TRAP(163)
	TRAP(164)
	TRAP(165)
	TRAP(166)
	TRAP(167)
	TRAP(168)
	TRAP(169)
	TRAP(170)
	TRAP(171)
	TRAP(172)
	TRAP(173)
	TRAP(174)
	TRAP(175)
	TRAP(176)
	TRAP(177)
	TRAP(178)
	TRAP(179)
	TRAP(180)
	TRAP(181)
	TRAP(182)
	TRAP(183)
	TRAP(184)
	TRAP(185)
	TRAP(186)
	TRAP(187)
	TRAP(188)
	TRAP(189)
	TRAP(190)
	TRAP(191)
	TRAP(192)
	TRAP(193)
	TRAP(194)
	TRAP(195)
	TRAP(196)
	TRAP(197)
	TRAP(198)
	TRAP(199)
	TRAP(200)
	TRAP(201)
	TRAP(202)
	TRAP(203)
	TRAP(204)
	TRAP(205)
	TRAP(206)
	TRAP(207)
	TRAP(208)
	TRAP(209)
	TRAP(210)
	TRAP(211)
	TRAP(212)
	TRAP(213)
	TRAP(214)
	TRAP(215)
	TRAP(216)
	TRAP(217)
	TRAP(218)
	TRAP(219)
	TRAP(220)
	TRAP(221)
	TRAP(222)
	TRAP(223)
	TRAP(224)
	TRAP(225)
	TRAP(226)
	TRAP(227)
	TRAP(228)
	TRAP(229)
	TRAP(230)
	TRAP(231)
	TRAP(232)
	TRAP(233)
	TRAP(234)
	TRAP(235)
	TRAP(236)
	TRAP(237)
	TRAP(238)
	TRAP(239)
	TRAP(240)
	TRAP(241)
	TRAP(242)
	TRAP(243)
	TRAP(244)
	TRAP(245)
	TRAP(246)
	TRAP(247)
	TRAP(248)
	TRAP(249)
	TRAP(250)
	TRAP(251)
	TRAP(252)
	TRAP(253)
	TRAP(254)
	TRAP(255)
