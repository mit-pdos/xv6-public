#include "types.h"
#include "kernel.hh"
#include "cpu.hh"
#include "cpputil.hh"
#include "spinlock.h"
#include "sperf.hh"

using namespace scopedperf;

ctrgroup_chain<tsc_ctr> *perfgroup;

void
initsperf()
{
  static tsc_ctr tsc;
  perfgroup = new ctrgroup_chain<tsc_ctr>(&tsc);
}
