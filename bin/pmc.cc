#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"
#include "amd64.h"
#include "pmc.hh"
#include "bits.hh"

#define CMN PERF_SEL_USR|PERF_SEL_OS|PERF_SEL_ENABLE

struct selector {
  const char* name;
  u64 sel;
};

static struct selector pmc_selector[] = {
  { "not halted",      CMN|0x76 },
  { "remote probes",   CMN|(0x4|0x8)<<8|0xec },
  { "L2 misses",       CMN|(0x2|0x8)<<8|0x7e },
  { "MAB requests",    CMN|(0x1)<<8|0x68 },
  { "MAB cycles",      CMN|(0x1)<<8|0x69 },
};

static const char*
valstr(u64 val)
{
  static char buf[32];

  if (val > 10*1000*1000*1000UL)
    snprintf(buf, sizeof(buf), "%lu G", val/(1000*1000*1000));
  else if (val > 10*1000*1000)
    snprintf(buf, sizeof(buf), "%lu M", val/(1000*1000));
  else if (val > 10*1000)
    snprintf(buf, sizeof(buf), "%lu k", val/1000);
  else
    snprintf(buf, sizeof(buf), "%lu", val);    

  return buf;
}

int
main(int ac, const char *av[])
{
  const char **xav;
  int pmci = 0;

  xav = &av[1];
  if (xav[0][0] == '-') {
    pmci = atoi(&xav[0][1]);
    xav = &xav[1];
  }

  pmc_count::config(pmc_selector[pmci].sel);
  pmc_count pmc0 = pmc_count::read(0);
  u64 t0 = rdtsc();

  int pid = fork(0);
  if (pid < 0) {
    fprintf(1, "pmc: fork failed\n");
    exit();
  }
  
  if (pid == 0) {
    exec(xav[0], xav);
    fprintf(1, "pmc: exec failed\n");
    exit();
  }

  wait();
  pmc_count pmc1 = pmc_count::read(0);
  u64 t1 = rdtsc();

  fprintf(1, "%s cycles\n", valstr(t1-t0));
  fprintf(1, "%s %s\n", valstr(pmc1.delta(pmc0).sum()),
          pmc_selector[pmci].name);
  exit();
}
