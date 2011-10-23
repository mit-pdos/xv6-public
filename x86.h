#pragma once
// Routines to let C code use special x86 instructions.

static inline u8
inb(u16 port)
{
  u8 data = 0;

  __asm volatile("inb %1,%0" : "=a" (data) : "d" (port));
  return data;
}

static inline void
outb(u16 port, u8 data)
{
  __asm volatile("outb %0,%1" : : "a" (data), "d" (port));
}

static inline void
stosb(void *addr, int data, int cnt)
{
  __asm volatile("cld; rep stosb" :
                 "=D" (addr), "=c" (cnt) :
                 "0" (addr), "1" (cnt), "a" (data) :
                 "memory", "cc");
}

static inline void
microdelay(u32 delay)
{
  
}

static inline u32
xchg32(volatile u32 *addr, u32 newval)
{
  u32 result;
  
  // The + in "+m" denotes a read-modify-write operand.
  __asm volatile("lock; xchgl %0, %1" :
                 "+m" (*addr), "=a" (result) :
                 "1" (newval) :
                 "cc");
  return result;
}

static inline u64
readrflags(void)
{
  u64 rflags;
  __asm volatile("pushfq; popq %0" : "=r" (rflags));
  return rflags;
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

static inline void
nop_pause(void)
{
  __asm volatile("pause" : :);
}
