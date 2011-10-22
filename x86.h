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
microdelay(u32 delay)
{
  
}
