static __inline void breakpoint(void) __attribute__((always_inline));
static __inline uchar inb(int port) __attribute__((always_inline));
static __inline void insb(int port, void *addr, int cnt) __attribute__((always_inline));
static __inline ushort inw(int port) __attribute__((always_inline));
static __inline void insw(int port, void *addr, int cnt) __attribute__((always_inline));
static __inline uint inl(int port) __attribute__((always_inline));
static __inline void insl(int port, void *addr, int cnt) __attribute__((always_inline));
static __inline void outb(int port, uchar data) __attribute__((always_inline));
static __inline void outsb(int port, const void *addr, int cnt) __attribute__((always_inline));
static __inline void outw(int port, ushort data) __attribute__((always_inline));
static __inline void outsw(int port, const void *addr, int cnt) __attribute__((always_inline));
static __inline void outsl(int port, const void *addr, int cnt) __attribute__((always_inline));
static __inline void outl(int port, uint data) __attribute__((always_inline));
static __inline void invlpg(void *addr) __attribute__((always_inline));
struct segdesc;
static __inline void lgdt(struct segdesc *p, int) __attribute__((always_inline));
struct gatedesc;
static __inline void lidt(struct gatedesc *p, int) __attribute__((always_inline));
static __inline void lldt(ushort sel) __attribute__((always_inline));
static __inline void ltr(ushort sel) __attribute__((always_inline));
static __inline void lcr0(uint val) __attribute__((always_inline));
static __inline uint rcr0(void) __attribute__((always_inline));
static __inline uint rcr2(void) __attribute__((always_inline));
static __inline void lcr3(uint val) __attribute__((always_inline));
static __inline uint rcr3(void) __attribute__((always_inline));
static __inline void lcr4(uint val) __attribute__((always_inline));
static __inline uint rcr4(void) __attribute__((always_inline));
static __inline void tlbflush(void) __attribute__((always_inline));
static __inline uint read_eflags(void) __attribute__((always_inline));
static __inline void write_eflags(uint eflags) __attribute__((always_inline));
static __inline uint read_ebp(void) __attribute__((always_inline));
static __inline uint read_esp(void) __attribute__((always_inline));
static __inline void cpuid(uint info, uint *eaxp, uint *ebxp, uint *ecxp, uint *edxp);
static __inline void cli(void) __attribute__((always_inline));
static __inline void sti(void) __attribute__((always_inline));

static __inline void
breakpoint(void)
{
	__asm __volatile("int3");
}

static __inline uchar
inb(int port)
{
	uchar data;
	__asm __volatile("inb %w1,%0" : "=a" (data) : "d" (port));
	return data;
}

static __inline void
insb(int port, void *addr, int cnt)
{
	__asm __volatile("cld\n\trepne\n\tinsb"			:
			 "=D" (addr), "=c" (cnt)		:
			 "d" (port), "0" (addr), "1" (cnt)	:
			 "memory", "cc");
}

static __inline ushort
inw(int port)
{
	ushort data;
	__asm __volatile("inw %w1,%0" : "=a" (data) : "d" (port));
	return data;
}

static __inline void
insw(int port, void *addr, int cnt)
{
	__asm __volatile("cld\n\trepne\n\tinsw"			:
			 "=D" (addr), "=c" (cnt)		:
			 "d" (port), "0" (addr), "1" (cnt)	:
			 "memory", "cc");
}

static __inline uint
inl(int port)
{
	uint data;
	__asm __volatile("inl %w1,%0" : "=a" (data) : "d" (port));
	return data;
}

static __inline void
insl(int port, void *addr, int cnt)
{
	__asm __volatile("cld\n\trepne\n\tinsl"			:
			 "=D" (addr), "=c" (cnt)		:
			 "d" (port), "0" (addr), "1" (cnt)	:
			 "memory", "cc");
}

static __inline void
outb(int port, uchar data)
{
	__asm __volatile("outb %0,%w1" : : "a" (data), "d" (port));
}

static __inline void
outsb(int port, const void *addr, int cnt)
{
	__asm __volatile("cld\n\trepne\n\toutsb"		:
			 "=S" (addr), "=c" (cnt)		:
			 "d" (port), "0" (addr), "1" (cnt)	:
			 "cc");
}

static __inline void
outw(int port, ushort data)
{
	__asm __volatile("outw %0,%w1" : : "a" (data), "d" (port));
}

static __inline void
outsw(int port, const void *addr, int cnt)
{
	__asm __volatile("cld\n\trepne\n\toutsw"		:
			 "=S" (addr), "=c" (cnt)		:
			 "d" (port), "0" (addr), "1" (cnt)	:
			 "cc");
}

static __inline void
outsl(int port, const void *addr, int cnt)
{
	__asm __volatile("cld\n\trepne\n\toutsl"		:
			 "=S" (addr), "=c" (cnt)		:
			 "d" (port), "0" (addr), "1" (cnt)	:
			 "cc");
}

static __inline void
outl(int port, uint data)
{
	__asm __volatile("outl %0,%w1" : : "a" (data), "d" (port));
}

static __inline void 
invlpg(void *addr)
{ 
	__asm __volatile("invlpg (%0)" : : "r" (addr) : "memory");
}  

static __inline void
lgdt(struct segdesc *p, int size)
{
	volatile ushort pd[3];
  
	pd[0] = size-1;
	pd[1] = (uint)p;
	pd[2] = (uint)p >> 16;

	asm volatile("lgdt (%0)" : : "g" (pd));
}

static __inline void
lidt(struct gatedesc *p, int size)
{
	volatile ushort pd[3];
  
	pd[0] = size-1;
	pd[1] = (uint)p;
	pd[2] = (uint)p >> 16;
  
	asm volatile("lidt (%0)" : : "g" (pd));
}

static __inline void
lldt(ushort sel)
{
	__asm __volatile("lldt %0" : : "r" (sel));
}

static __inline void
ltr(ushort sel)
{
	__asm __volatile("ltr %0" : : "r" (sel));
}

static __inline void
lcr0(uint val)
{
	__asm __volatile("movl %0,%%cr0" : : "r" (val));
}

static __inline uint
rcr0(void)
{
	uint val;
	__asm __volatile("movl %%cr0,%0" : "=r" (val));
	return val;
}

static __inline uint
rcr2(void)
{
	uint val;
	__asm __volatile("movl %%cr2,%0" : "=r" (val));
	return val;
}

static __inline void
lcr3(uint val)
{
	__asm __volatile("movl %0,%%cr3" : : "r" (val));
}

static __inline uint
rcr3(void)
{
	uint val;
	__asm __volatile("movl %%cr3,%0" : "=r" (val));
	return val;
}

static __inline void
lcr4(uint val)
{
	__asm __volatile("movl %0,%%cr4" : : "r" (val));
}

static __inline uint
rcr4(void)
{
	uint cr4;
	__asm __volatile("movl %%cr4,%0" : "=r" (cr4));
	return cr4;
}

static __inline void
tlbflush(void)
{
	uint cr3;
	__asm __volatile("movl %%cr3,%0" : "=r" (cr3));
	__asm __volatile("movl %0,%%cr3" : : "r" (cr3));
}

static __inline uint
read_eflags(void)
{
        uint eflags;
        __asm __volatile("pushfl; popl %0" : "=r" (eflags));
        return eflags;
}

static __inline void
write_eflags(uint eflags)
{
        __asm __volatile("pushl %0; popfl" : : "r" (eflags));
}

static __inline uint
read_ebp(void)
{
        uint ebp;
        __asm __volatile("movl %%ebp,%0" : "=r" (ebp));
        return ebp;
}

static __inline uint
read_esp(void)
{
        uint esp;
        __asm __volatile("movl %%esp,%0" : "=r" (esp));
        return esp;
}

static __inline uint
read_esi(void)
{
        uint esi;
        __asm __volatile("movl %%esi,%0" : "=r" (esi));
        return esi;
}

static __inline uint
read_edi(void)
{
        uint edi;
        __asm __volatile("movl %%edi,%0" : "=r" (edi));
        return edi;
}

static __inline uint
read_ebx(void)
{
        uint ebx;
        __asm __volatile("movl %%ebx,%0" : "=r" (ebx));
        return ebx;
}

static __inline void
cpuid(uint info, uint *eaxp, uint *ebxp, uint *ecxp, uint *edxp)
{
	uint eax, ebx, ecx, edx;
	asm volatile("cpuid" 
		: "=a" (eax), "=b" (ebx), "=c" (ecx), "=d" (edx)
		: "a" (info));
	if (eaxp)
		*eaxp = eax;
	if (ebxp)
		*ebxp = ebx;
	if (ecxp)
		*ecxp = ecx;
	if (edxp)
		*edxp = edx;
}

static __inline uint
cmpxchg(uint oldval, uint newval, volatile uint* lock_addr)
{
  uint result;
  __asm__ __volatile__(
		       "lock; cmpxchgl %2, %0"
		       :"+m" (*lock_addr), "=a" (result) : "r"(newval), "1"(oldval) : "cc"
		       );
  return result;
}

static __inline void
cli(void)
{
	__asm__ volatile("cli");
}

static __inline void
sti(void)
{
	__asm__ volatile("sti");
}

struct trapframe {
    /* registers as pushed by pusha */
    uint edi;
    uint esi;
    uint ebp;
    uint oesp;      /* Useless */
    uint ebx;
    uint edx;
    uint ecx;
    uint eax;
    /* rest of trap frame */
    ushort es;
    ushort padding1;
    ushort ds;
    ushort padding2;
    uint trapno;
    /* below here defined by x86 hardware */
    uint err;
    uint eip;
    ushort cs;
    ushort padding3;
    uint eflags;
    /* below here only when crossing rings, such as from user to kernel */
    uint esp;
    ushort ss;
    ushort padding4;
};

#define MAX_IRQS	16	// Number of IRQs

#define IRQ_OFFSET      32	// IRQ 0 corresponds to int IRQ_OFFSET

#define IRQ_KBD         1
#define IRQ_IDE         14
#define IRQ_ERROR       19
#define IRQ_SPURIOUS    31
