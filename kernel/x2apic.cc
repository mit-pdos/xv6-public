#include "types.h"
#include "amd64.h"
#include "bits.hh"
#include "kernel.hh"

void
x2apicstartap(hwid_t id, u32 addr)
{
  panic("x2apicstartap");
}

void
x2apiceoi(void)
{
  panic("x2apiceoi");
}

void
x2apic_tlbflush(hwid_t id)
{
  panic("x2apic_tlbflush");
}

void
x2apic_sampconf(hwid_t id)
{
  panic("x2apic_sampconf");
}

void
x2apicpc(char mask)
{
  panic("x2apicpc");
}

hwid_t
x2apicid(void)
{
  panic("x2apicid");
  return HWID(0);
}

void
initx2apic(void)
{
  panic("initx2apic");
}
