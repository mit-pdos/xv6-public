// SYSCALL and SYSRET registers
#define MSR_STAR        0xc0000081
#define MSR_LSTAR       0xc0000082
#define MSR_CSTAR       0xc0000083
#define MSR_SFMASK      0xc0000084

// GS
#define MSR_GS_BASE     0xc0000101
#define MSR_GS_KERNBASE 0xc0000102

static inline uint64
readmsr(uint32 msr)
{
  uint32 hi, lo;
  __asm volatile("rdmsr" : "=d" (hi), "=a" (lo) : "c" (msr));
  return ((uint64) lo) | (((uint64) hi) << 32);
}

static inline void
writemsr(uint64 msr, uint64 val)
{
  uint32 lo = val & 0xffffffff;
  uint32 hi = val >> 32;
  __asm volatile("wrmsr" : : "c" (msr), "a" (lo), "d" (hi) : "memory");
}
