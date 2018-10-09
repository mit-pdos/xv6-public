// Routines to let C code use special x86 instructions.

#ifndef __ASSEMBLER__

static inline uchar
inb(ushort port)
{
  uchar data;

  asm volatile("in %1,%0" : "=a" (data) : "d" (port));
  return data;
}

static inline void
insl(int port, void *addr, int cnt)
{
  asm volatile("cld; rep insl" :
               "=D" (addr), "=c" (cnt) :
               "d" (port), "0" (addr), "1" (cnt) :
               "memory", "cc");
}

static inline void
outb(ushort port, uchar data)
{
  asm volatile("out %0,%1" : : "a" (data), "d" (port));
}

static inline void
outw(ushort port, ushort data)
{
  asm volatile("out %0,%1" : : "a" (data), "d" (port));
}

static inline void
outsl(int port, const void *addr, int cnt)
{
  asm volatile("cld; rep outsl" :
               "=S" (addr), "=c" (cnt) :
               "d" (port), "0" (addr), "1" (cnt) :
               "cc");
}

static inline void
stosb(void *addr, int data, int cnt)
{
  asm volatile("cld; rep stosb" :
               "=D" (addr), "=c" (cnt) :
               "0" (addr), "1" (cnt), "a" (data) :
               "memory", "cc");
}

static inline void
stosl(void *addr, int data, int cnt)
{
  asm volatile("cld; rep stosl" :
               "=D" (addr), "=c" (cnt) :
               "0" (addr), "1" (cnt), "a" (data) :
               "memory", "cc");
}

static inline void
lgdt(void *p)
{
  asm volatile("lgdt (%0)" : : "r" (p) : "memory");
}

static inline void
lidt(void *p)
{
  asm volatile("lidt (%0)" : : "r" (p) : "memory");
}

static inline void
ltr(ushort sel)
{
  asm volatile("ltr %0" : : "r" (sel));
}

static inline uint64
readeflags(void)
{
  uint64 eflags;
  asm volatile("pushf; pop %0" : "=r" (eflags));
  return eflags;
}

static inline void
loadgs(ushort v)
{
  asm volatile("movw %0, %%gs" : : "r" (v));
}

static inline void
cli(void)
{
  asm volatile("cli");
}

static inline void
sti(void)
{
  asm volatile("sti");
}

static inline uint
xchg(volatile uint *addr, uint newval)
{
  uint result;

  // The + in "+m" denotes a read-modify-write operand.
  asm volatile("lock; xchgl %0, %1" :
               "+m" (*addr), "=a" (result) :
               "1" (newval) :
               "cc");
  return result;
}

static inline uint
rcr2(void)
{
  uint64 val;
  asm volatile("mov %%cr2,%0" : "=r" (val));
  return val;
}

static inline void
lcr3(uint64 val)
{
  asm volatile("mov %0,%%cr3" : : "r" (val));
}

static inline void
writegs(uint16 v)
{
  __asm volatile("movw %0, %%gs" : : "r" (v));
}


//PAGEBREAK: 36
// Layout of the trap frame built on the stack by the
// hardware and by trapasm.S, and passed to trap().
struct trapframe {
   uint64 rax;      
   uint64 rbx;
   uint64 rcx;
   uint64 rdx;
   uint64 rbp;
   uint64 rsi;
   uint64 rdi;
   uint64 r8;
   uint64 r9;
   uint64 r10;
   uint64 r11;
   uint64 r12;
   uint64 r13;
   uint64 r14;
   uint64 r15;
   uint64 trapno;
   uint64 err;
   uint64 rip;     
   uint16 cs;
   uint16 padding[3];
   uint64 rflags;  
   uint64 rsp;     
   uint64 ss;      
}__attribute__((packed));

struct sysframe {
  // arguments
  uint64 rdi;
  uint64 rsi;
  uint64 rdx;
  uint64 r10;
  uint64 r8;
  uint64 r9;
  
  // callee-saved registers
  uint64 r15;
  uint64 r14;
  uint64 r13;
  uint64 r12;
  uint64 rbx;
  uint64 rbp;

  // return value
  uint64 rax;

  // syscall registers
  uint64 r11;   // eflags
  uint64 rcx;   // rip
  uint64 rsp;
  
}__attribute__((packed));

#endif

#define TF_CS 144 // offset in trapframe for saved cs
