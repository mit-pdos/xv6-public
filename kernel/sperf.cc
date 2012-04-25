#include "types.h"
#include "kernel.hh"
#include "cpu.hh"
#include "cpputil.hh"
#include "spinlock.h"
#include "sperf.hh"

using namespace scopedperf;

static tsc_ctr tsc;
ctrgroup_chain<tsc_ctr> perfgroup(&tsc);
