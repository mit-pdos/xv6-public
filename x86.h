#pragma once
// Routines to let C code use special x86 instructions.

static inline uchar
inb(ushort port)
{
  uchar data;

  __asm volatile("in %1,%0" : "=a" (data) : "d" (port));
  return data;
}

static inline void
insl(int port, void *addr, int cnt)
{
  __asm volatile("cld; rep insl" :
                 "=D" (addr), "=c" (cnt) :
                 "d" (port), "0" (addr), "1" (cnt) :
                 "memory", "cc");
}

static inline void
outb(ushort port, uchar data)
{
  __asm volatile("out %0,%1" : : "a" (data), "d" (port));
}

static inline void
outw(ushort port, ushort data)
{
  __asm volatile("out %0,%1" : : "a" (data), "d" (port));
}

static inline void
outsl(int port, const void *addr, int cnt)
{
  __asm volatile("cld; rep outsl" :
                 "=S" (addr), "=c" (cnt) :
                 "d" (port), "0" (addr), "1" (cnt) :
                 "cc");
}

static inline void
stosb(void *addr, int data, int cnt)
{
  __asm volatile("cld; rep stosb" :
                 "=D" (addr), "=c" (cnt) :
                 "0" (addr), "1" (cnt), "a" (data) :
                 "memory", "cc");
}

struct segdesc;

static inline void
lgdt(struct segdesc *p, int size)
{
  volatile ushort pd[3];

  pd[0] = size-1;
  pd[1] = (uint)p;
  pd[2] = (uint)p >> 16;

  __asm volatile("lgdt (%0)" : : "r" (pd));
}

struct gatedesc;

static inline void
lidt(struct gatedesc *p, int size)
{
  volatile ushort pd[3];

  pd[0] = size-1;
  pd[1] = (uint)p;
  pd[2] = (uint)p >> 16;

  __asm volatile("lidt (%0)" : : "r" (pd));
}

static inline void
ltr(ushort sel)
{
  __asm volatile("ltr %0" : : "r" (sel));
}

static inline uint
readeflags(void)
{
  uint eflags;
  __asm volatile("pushfl; popl %0" : "=r" (eflags));
  return eflags;
}

static inline void
loadgs(ushort v)
{
  __asm volatile("movw %0, %%gs" : : "r" (v));
}

static inline uint
rebp(void)
{
  uint val;
  __asm volatile("movl %%ebp,%0" : "=r" (val));
  return val;
}

static inline uint
resp(void)
{
  uint val;
  __asm volatile("movl %%esp,%0" : "=r" (val));
  return val;
}

static inline void
cli(void)
{
  __asm volatile("cli");
}

static inline void
sti(void)
{
  __asm volatile("sti");
}

static inline uint
xchg(volatile uint *addr, uint newval)
{
  uint result;
  
  // The + in "+m" denotes a read-modify-write operand.
  __asm volatile("lock; xchgl %0, %1" :
                 "+m" (*addr), "=a" (result) :
                 "1" (newval) :
                 "cc");
  return result;
}

static inline void
nop_pause(void)
{
  __asm volatile("pause" : :);
}

//PAGEBREAK!
static inline void
lcr0(uint val)
{
  __asm volatile("movl %0,%%cr0" : : "r" (val));
}

static inline uint
rcr0(void)
{
  uint val;
  __asm volatile("movl %%cr0,%0" : "=r" (val));
  return val;
}

static inline uint
rcr2(void)
{
  uint val;
  __asm volatile("movl %%cr2,%0" : "=r" (val));
  return val;
}

static inline void
lcr3(uint val) 
{
  __asm volatile("movl %0,%%cr3" : : "r" (val));
}

static inline uint
rcr3(void)
{
  uint val;
  __asm volatile("movl %%cr3,%0" : "=r" (val));
  return val;
}

static __inline__ unsigned long long rdtsc(void)
{
  unsigned hi, lo;
  __asm__ __volatile__ ("rdtsc" : "=a"(lo), "=d"(hi));
  return ( (unsigned long long)lo)|( ((unsigned long long)hi)<<32 );
}

//PAGEBREAK: 36
// Layout of the trap frame built on the stack by the
// hardware and by trapasm.S, and passed to trap().
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
  ushort gs;
  ushort padding1;
  ushort fs;
  ushort padding2;
  ushort es;
  ushort padding3;
  ushort ds;
  ushort padding4;
  uint trapno;

  // below here defined by x86 hardware
  uint err;
  uint eip;
  ushort cs;
  ushort padding5;
  uint eflags;

  // below here only when crossing rings, such as from user to kernel
  uint esp;
  ushort ss;
  ushort padding6;
};
