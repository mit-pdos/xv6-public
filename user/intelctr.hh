namespace intelctr {

using scopedperf::tsc_ctr;
using scopedperf::pmc_setup;

static tsc_ctr tsc;

static pmc_setup<48> l2_ld_hit(0x00410124, "l2 ld hit");
static pmc_setup<48> l2_ld_miss(0x00410224, "l2 ld miss");

// rfo: request for ownership (~write)
static pmc_setup<48> l2_rfo_hit(0x00410424, "l2 rfo hit");
static pmc_setup<48> l2_rfo_miss(0x00410824, "l2 rfo miss");

static pmc_setup<48> l2_i_hit(0x00411024, "l2 i hit");
static pmc_setup<48> l2_i_miss(0x00412024, "l2 i miss");

static pmc_setup<48> l2_prefetch_hit(0x00414024, "l2 pref hit");
static pmc_setup<48> l2_prefetch_miss(0x00418024, "l2 pref miss");
static pmc_setup<48> l2_prefetch(0x0041c024, "l2 prefetch");  // ~zero

static pmc_setup<48> l2_miss(0x0041aa24, "l2 all miss");
static pmc_setup<48> l2_refs(0x0041ff24, "l2 all refs");

// ---
static pmc_setup<48> l2_ld_demand(0x00410f26, "l2 demand ld");
static pmc_setup<48> l2_ld_demand_i(0x00410126, "l2 dem ld i");  // ~l2_ld_miss
static pmc_setup<48> l2_ld_demand_s(0x00410226, "l2 dem ld s");  // ~l2_rfo_miss
static pmc_setup<48> l2_ld_demand_e(0x00410426, "l2 dem ld e");
static pmc_setup<48> l2_ld_demand_m(0x00410826, "l2 dem ld m");
static pmc_setup<48> l2_ld_prefetch(0x0041f026, "l2 prefetch ld");  // ~zero

// ---
static pmc_setup<48> l2_wr_i(0x00410127, "l2 write i");
static pmc_setup<48> l2_wr_s(0x00410227, "l2 write s");
static pmc_setup<48> l2_wr_m(0x00410827, "l2 write m");
static pmc_setup<48> l2_wr_sem(0x00410e27, "l2 write sem");
static pmc_setup<48> l2_wr(0x00410f27, "l2 write");  // l2_wr_i + l2_wr_sem
static pmc_setup<48> l2_wrlk(0x0041f027, "l2 wrlk");  // ??

// ---
// where do loads come from?  interesting, but maybe inaccurate?
//   doesn't add up to other l2 counters..
static pmc_setup<48> ld_l1hit(0x004101cb, "ld l1 hit");
static pmc_setup<48> ld_l2hit(0x004102cb, "ld l2 hit");
static pmc_setup<48> ld_l3hit_unsh(0x004104cb, "ld l3 unsh");
static pmc_setup<48> ld_l2other(0x004108cb, "ld l2 other");
static pmc_setup<48> ld_offdie(0x004110cb, "ld offdie");
static pmc_setup<48> ld_lfb(0x004140cb, "ld lfb");
static pmc_setup<48> ld_dtlbmiss(0x004180cb, "ld dtlb-miss");

// ---
static pmc_setup<48> uops(0x0041010e, "uops_issued");
static pmc_setup<48> mem_loads(0x0041010b, "mem load ins");
static pmc_setup<48> mem_stores(0x0041020b, "mem store ins");
static pmc_setup<48> dtlb_miss(0x00410149, "dtlb miss");
static pmc_setup<48> itlb_miss(0x00410185, "itlb miss");

}
