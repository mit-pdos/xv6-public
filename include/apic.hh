#if defined(HW_ben)
#define lapicstartap   x2apicstartap
#define lapiceoi       x2apiceoi
#define lapic_tlbflush x2apic_tlbflush
#define lapic_sampconf x2apic_sampconf
#define lapicpc        x2apicpc
#define lapicid        x2apicid
#define initlapic      initx2apic
#else
#define lapicstartap   xapicstartap
#define lapiceoi       xapiceoi
#define lapic_tlbflush xapic_tlbflush
#define lapic_sampconf xapic_sampconf
#define lapicpc        xapicpc
#define lapicid        xapicid
#define initlapic      initxapic
#endif

// xapic.cc
void            xapicstartap(hwid_t, u32 addr);
void            xapiceoi(void);
void            xapic_tlbflush(hwid_t);
void            xapic_sampconf(hwid_t);
void            xapicpc(char mask);
hwid_t          xapicid(void);
void            initxapic(void);

// x2apic.cc
void            x2apicstartap(hwid_t, u32 addr);
void            x2apiceoi(void);
void            x2apic_tlbflush(hwid_t);
void            x2apic_sampconf(hwid_t);
void            x2apicpc(char mask);
hwid_t          x2apicid(void);
void            initx2apic(void);
