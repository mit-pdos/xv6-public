// Special assembly routines to access x86-specific
// hardware instructions.

static __inline uchar
inb(int port)
{
  uchar data;
  __asm __volatile("inb %w1,%0" : "=a" (data) : "d" (port));
  return data;
}

static __inline void
insl(int port, void *addr, int cnt)
{
  __asm __volatile("cld\n\trepne\n\tinsl"     :
                   "=D" (addr), "=c" (cnt)    :
                   "d" (port), "0" (addr), "1" (cnt)  :
                   "memory", "cc");
}

static __inline void
outb(int port, uchar data)
{
  __asm __volatile("outb %0,%w1" : : "a" (data), "d" (port));
}

static __inline void
outw(int port, ushort data)
{
  __asm __volatile("outw %0,%w1" : : "a" (data), "d" (port));
}

static __inline void
outsl(int port, const void *addr, int cnt)
{
  __asm __volatile("cld\n\trepne\n\toutsl"    :
                   "=S" (addr), "=c" (cnt)    :
                   "d" (port), "0" (addr), "1" (cnt)  :
                   "cc");
}

struct segdesc;

static __inline void
lgdt(struct segdesc *p, int size)
{
  volatile ushort pd[3];

  pd[0] = size-1;
  pd[1] = (uint)p;
  pd[2] = (uint)p >> 16;

  asm volatile("lgdt (%0)" : : "g" (pd));
}

struct gatedesc;

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
ltr(ushort sel)
{
  __asm __volatile("ltr %0" : : "r" (sel));
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

static __inline void
cpuid(uint info, uint *eaxp, uint *ebxp, uint *ecxp, uint *edxp)
{
  uint eax, ebx, ecx, edx;
  asm volatile("cpuid" :
               "=a" (eax), "=b" (ebx), "=c" (ecx), "=d" (edx) :
               "a" (info));
  if(eaxp)
    *eaxp = eax;
  if(ebxp)
    *ebxp = ebx;
  if(ecxp)
    *ecxp = ecx;
  if(edxp)
    *edxp = edx;
}

static __inline uint
cmpxchg(uint oldval, uint newval, volatile uint* lock_addr)
{
  uint result;
  __asm__ __volatile__("lock; cmpxchgl %2, %0" :
                       "+m" (*lock_addr), "=a" (result) :
                       "r"(newval), "1"(oldval) :
                       "cc");
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

// Layout of the trap frame on the stack upon entry to trap.
struct trapframe {
  // registers as pushed by pusha
  uint edi;
  uint esi;
  uint ebp;
  uint oesp;      // useless & ignored
  uint ebx;
  uint edx;
  uint ecx;
  uint eax;

  // rest of trap frame
  ushort es;
  ushort padding1;
  ushort ds;
  ushort padding2;
  uint trapno;

  // below here defined by x86 hardware
  uint err;
  uint eip;
  ushort cs;
  ushort padding3;
  uint eflags;

  // below here only when crossing rings, such as from user to kernel
  uint esp;
  ushort ss;
  ushort padding4;
};
