#include "types.h"
#include "kernel.hh"
#include "cpu.hh"
#include "cpputil.hh"
#include "spinlock.h"
#include "sperf.hh"

using namespace scopedperf;

static tsc_ctr tsc;
ctrgroup_chain<tsc_ctr> perfgroup(&tsc);
// but xv6 doesn't run static constructors, so call them explicitly below..

void
initsperf()
{
  new (&tsc) tsc_ctr();
  new (&perfgroup) ctrgroup_chain<tsc_ctr>(&tsc);
}
