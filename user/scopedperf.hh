#pragma once

/*
 * Canonical location:
 *   git+ssh://amsterdam.csail.mit.edu/home/am1/prof/proftools.git
 *   under spmc/lib/scopedperf.hh
 */

#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <algorithm>
#include <assert.h>
#include <string.h>
#include <stdint.h>
#include <sys/time.h>

namespace scopedperf {

/*
 * statically enable/disable most of the generated code for profiling.
 */
class default_enabler {
 public:
  bool enabled() const { return true; }
};

class always_enabled {
 public:
  bool enabled() const { return true; }
};

class always_disabled {
 public:
  bool enabled() const { return false; }
};


/*
 * spinlock: mostly to avoid pthread mutex sleeping.
 */
class spinlock {
 public:
  spinlock() : x(0) {}

  void acquire() {
    while (!__sync_bool_compare_and_swap(&x, 0, 1))
      ;
  }

  void release() {
    x = 0;
  }

 private:
  volatile uint x;
};

class scoped_spinlock {
 public:
  scoped_spinlock(spinlock *larg) : l(larg) {
    l->acquire();
    held = true;
  }

  void release() {
    if (held)
      l->release();
    held = false;
  }

  ~scoped_spinlock() { release(); }

 private:
  spinlock *const l;
  bool held;
};


/*
 * ctrgroup: a group of performance counters.
 */

template<typename... Counters>
class ctrgroup_chain;

template<>
class ctrgroup_chain<> {
 public:
  ctrgroup_chain() {}
  static const uint nctr = 0;
  void get_samples(uint64_t *v) const {}
  void get_delta(uint64_t *delta, uint64_t *prev) const {}
  std::vector<std::string> get_names() const { return {}; }
};

template<typename One, typename... Others>
class ctrgroup_chain<One, Others...> : ctrgroup_chain<Others...> {
 public:
  ctrgroup_chain(One *x, Others*... y)
    : ctrgroup_chain<Others...>(y...), ctr(x)
  {
    x->setup();
  }

  static const uint nctr = 1 + ctrgroup_chain<Others...>::nctr;

  void get_samples(uint64_t *v) const {
    v[0] = ctr->sample();
    ctrgroup_chain<Others...>::get_samples(v+1);
  }

  void get_delta(uint64_t *delta, uint64_t *prev) const {
    uint64_t x = ctr->sample();
    *delta = (x - *prev) & ctr->mask;
    *prev = x;
    ctrgroup_chain<Others...>::get_delta(delta+1, prev+1);
  }

  std::vector<std::string> get_names() const {
    std::vector<std::string> v = ctrgroup_chain<Others...>::get_names();
    v.insert(v.begin(), ctr->name);
    return v;
  }

 private:
  const One *const ctr;
};

template<typename... Counters>
ctrgroup_chain<Counters...>
ctrgroup(Counters*... args)
{
  return ctrgroup_chain<Counters...>(args...);
}


/*
 * perfsum: aggregating counter deltas across multiple CPUs.
 */
class perfsum_base {
 public:
  enum display_opt { show, hide };

  perfsum_base(const std::string &n, display_opt d) : name(n), disp(d) {
    scoped_spinlock x(get_sums_lock());
    get_sums()->push_back(this);
  }

  static void printall(int w0 = 17, int w = 13) {
    scoped_spinlock x(get_sums_lock());
    auto sums = get_sums();
    std::sort(sums->begin(), sums->end(),
	      [](perfsum_base *a, perfsum_base *b) { return a->name < b->name; });
    for (perfsum_base *ps: *sums) {
      if (ps->disp == hide || !ps->get_enabled())
	continue;
      auto p = ps->get_stats();
      print_row(ps->name, ps->get_names(), w0, w, [](const std::string &name)
		{ return name; });
      print_row("  avg",   p, w0, w, [](const std::pair<uint64_t, uint64_t> &e)
	        { return ((double) e.second) / (double) e.first; });
      print_row("  total", p, w0, w, [](const std::pair<uint64_t, uint64_t> &e)
		{ return e.second; });
      print_row("  count", p, w0, w, [](const std::pair<uint64_t, uint64_t> &e)
		{ return e.first; });
    }
  }

  static void resetall() {
    scoped_spinlock x(get_sums_lock());
    for (perfsum_base *ps: *get_sums())
      ps->reset();
  }

  virtual std::vector<std::pair<uint64_t, uint64_t> > get_stats() const = 0;
  virtual std::vector<std::string> get_names() const = 0;
  virtual bool get_enabled() const = 0;
  virtual void reset() = 0;

 private:
  template<class Row, class Callback>
  static void print_row(const std::string &rowname, const Row &r,
			int w0, int w, Callback f)
  {
    std::cout << std::left << std::setw(w0) << rowname;
    for (const auto &elem: r)
      std::cout << std::left << std::setw(w) << f(elem) << " ";
    std::cout << std::endl;
  }

  static std::vector<perfsum_base*> *get_sums() {
    static std::vector<perfsum_base*> v;
    return &v;
  }

  static spinlock *get_sums_lock() {
    static spinlock l;
    return &l;
  }

  const std::string name;
  const display_opt disp;
};

static inline void
compiler_barrier()
{
  /* Avoid compile-time reordering across performance counter reads */
  __asm __volatile("" ::: "memory");
}

template<typename Enabler, typename... Counters>
class perfsum_ctr : public perfsum_base, public Enabler {
 public:
  perfsum_ctr(const ctrgroup_chain<Counters...> *c,
	      const std::string &n, display_opt d)
    : perfsum_base(n, d), cg(c), base(0)
  {
    reset();
  }

  perfsum_ctr(const std::string &n,
	      const perfsum_ctr<Enabler, Counters...> *basesum, display_opt d)
    : perfsum_base(n, d), cg(basesum->cg), base(basesum)
  {
    reset();
  }

  void get_samples(uint64_t *s) const {
    compiler_barrier();
    cg->get_samples(s);
    compiler_barrier();
  }

  void record(uint cpuid, uint64_t *s) {
    uint64_t delta[cg->nctr];

    compiler_barrier();
    cg->get_delta(delta, s);
    compiler_barrier();

    for (uint i = 0; i < cg->nctr; i++)
      stat[cpuid].sum[i] += delta[i];
    stat[cpuid].count++;
  }

  std::vector<std::pair<uint64_t, uint64_t> > get_stats() const /* override */ {
    std::vector<std::pair<uint64_t, uint64_t> > v;
    for (uint i = 0; i < cg->nctr; i++) {
      uint64_t b =
	base ? base->addcpus([i](const stats *s) { return s->sum[i]; })
	     : addcpus([](const stats *s) { return s->count; });
      v.push_back(std::make_pair(b,
	addcpus([i](const stats *s) { return s->sum[i]; })));
    }
    return v;
  }

  std::vector<std::string> get_names() const /* override */ {
    return cg->get_names();
  }

  bool get_enabled() const /* override */ {
    return Enabler::enabled();
  }

  void reset() /* override */ {
    memset(stat, 0, sizeof(stat));
  }

 private:
  enum { maxcpu = 256 };

  struct stats {
    uint64_t count;
    uint64_t sum[ctrgroup_chain<Counters...>::nctr];
  } __attribute__((aligned (128)));

  struct stats stat[maxcpu];
  const struct ctrgroup_chain<Counters...> *const cg;
  const struct perfsum_ctr<Enabler, Counters...> *const base;

  template<class T>
  uint64_t addcpus(T f) const {
    uint64_t tot = 0;
    for (uint i = 0; i < maxcpu; i++)
      tot += f(&stat[i]);
    return tot;
  }
};

template<typename Enabler, typename... Counters>
class perfsum_ctr_inlinegroup :
  public ctrgroup_chain<Counters...>,
  public perfsum_ctr<Enabler, Counters...>
{
 public:
  perfsum_ctr_inlinegroup(const std::string &n, perfsum_base::display_opt d,
                          Counters*... ctrs)
    : ctrgroup_chain<Counters...>(ctrs...),
      perfsum_ctr<Enabler, Counters...>(this, n, d) {}
};

template<typename Enabler = default_enabler, typename... Counters>
perfsum_ctr<Enabler, Counters...>
perfsum(const std::string &name, const ctrgroup_chain<Counters...> *c,
	const perfsum_base::display_opt d = perfsum_base::show)
{
  return perfsum_ctr<Enabler, Counters...>(c, name, d);
}

template<typename Enabler = default_enabler, typename... Counters>
perfsum_ctr_inlinegroup<Enabler, Counters...>
perfsum_group(const std::string &name, Counters*... c)
{
  return perfsum_ctr_inlinegroup<Enabler, Counters...>(name, perfsum_base::show, c...);
}

template<typename Enabler, typename... Counters>
perfsum_ctr<Enabler, Counters...>
perfsum_frac(const std::string &name,
	     const perfsum_ctr<Enabler, Counters...> *base)
{
  return perfsum_ctr<Enabler, Counters...>(name, base, perfsum_base::show);
}


/*
 * namedctr &c: actual counter implementations.
 */
template<uint64_t CounterWidth>
class namedctr {
 public:
  namedctr(const std::string &n) : name(n) {}
  void setup() {}
  const std::string name;
  static const uint64_t mask =
    ((1ULL << (CounterWidth - 1)) - 1) << 1 | 1;
};

class tsc_ctr : public namedctr<64> {
 public:
  tsc_ctr() : namedctr("tsc") {}
  static uint64_t sample() {
    uint64_t a, d;
    __asm __volatile("rdtsc" : "=a" (a), "=d" (d));
    return a | (d << 32);
  }
};

class tscp_ctr : public namedctr<64> {
 public:
  tscp_ctr() : namedctr("tscp") {}
  static uint64_t sample() {
    uint64_t a, d, c;
    __asm __volatile("rdtscp" : "=a" (a), "=d" (d), "=c" (c));
    return a | (d << 32);
  }
};

template<uint64_t CounterWidth>
class pmc_ctr : public namedctr<CounterWidth> {
 public:
  pmc_ctr(int n) : namedctr<CounterWidth>(mkname(n)), cn(n) {}
  pmc_ctr(const std::string &nm) : namedctr<CounterWidth>(nm), cn(-1) {}

  uint64_t sample() const {
    uint64_t a, d;
    __asm __volatile("rdpmc" : "=a" (a), "=d" (d) : "c" (cn));
    return a | (d << 32);
  }

  int cn;

 private:
  static std::string mkname(int n) {
    std::stringstream ss;
    ss << "pmc" << n;
    return ss.str();
  }
};

template<uint64_t CounterWidth = 64>
class pmc_setup : public pmc_ctr<CounterWidth> {
 public:
  pmc_setup(uint64_t v, const std::string &nm)
    : pmc_ctr<CounterWidth>(nm), pmc_v(v) {}

  void setup() {
    if (pmc_ctr<CounterWidth>::cn >= 0)
      return;

    /*
     * XXX detect how many counters the hardware has
     */
    static bool pmcuse[4];
    static spinlock pmcuselock;

    int n = 0;
    scoped_spinlock x(&pmcuselock);
    while (n < 4 && pmcuse[n])
      n++;
    assert(n < 4);
    pmcuse[n] = true;
    x.release();

    // ugly but effective
    std::stringstream ss;
    ss << "for f in /sys/kernel/spmc/cpu*/" << n << "; do "
       << "echo " << std::hex << pmc_v << " > $f; done";
    assert(0 == system(ss.str().c_str()));

    pmc_ctr<CounterWidth>::cn = n;
  }

 private:
  uint64_t pmc_v;
};

class tod_ctr : public namedctr<64> {
 public:
  tod_ctr() : namedctr("tod-usec") {}
  uint64_t sample() const {
    struct timeval tv;
    gettimeofday(&tv, 0);
    return ((uint64_t) tv.tv_usec) + ((uint64_t) tv.tv_sec) * 1000000;
  }
};

class zero_ctr : public namedctr<64> {
 public:
  zero_ctr() : namedctr("zero") {}
  uint64_t sample() const { return 0; }
};


/*
 * scoped performance-counting regions, which record samples into a perfsum.
 */
template<typename Enabler, typename... Counters>
class base_perf_region {
 public:
  base_perf_region(perfsum_ctr<Enabler, Counters...> *psarg)
    : ps(psarg), enabled(ps->enabled()), cpuid(enabled ? sched_getcpu() : 0)
  {
    if (enabled)
      ps->get_samples(s);
  }

  // invoke lap multiple times to precisely measure iterations
  // (use same measurement for end of one & start of next round)
  void lap() {
    if (enabled)
      ps->record(cpuid, s);
  }

 private:
  perfsum_ctr<Enabler, Counters...> *const ps;
  const bool enabled;
  const uint cpuid;
  uint64_t s[ctrgroup_chain<Counters...>::nctr];
};

template<typename Enabler, typename... Counters>
class scoped_perf_region : public base_perf_region<Enabler, Counters...> {
 public:
  scoped_perf_region(perfsum_ctr<Enabler, Counters...> *psarg)
    : base_perf_region<Enabler, Counters...>(psarg) {}
  ~scoped_perf_region() { base_perf_region<Enabler, Counters...>::lap(); }
};

template<typename Enabler, typename... Counters>
class killable_perf_region : public base_perf_region<Enabler, Counters...> {
 public:
  killable_perf_region(perfsum_ctr<Enabler, Counters...> *psarg)
    : base_perf_region<Enabler, Counters...>(psarg), active(true) {}
  ~killable_perf_region() { stop(); }

  // perform a final measurement, if needed before destructor
  void stop() {
    if (active)
      base_perf_region<Enabler, Counters...>::lap();
    active = false;
  }

  // prevent destructor from performing a measurement
  void kill() { active = false; }

 private:
  bool active;
};

template<typename Enabler, typename... Counters>
scoped_perf_region<Enabler, Counters...>
perf_region(perfsum_ctr<Enabler, Counters...> *ps)
{
  return scoped_perf_region<Enabler, Counters...>(ps);
}

template<typename Enabler, typename... Counters>
killable_perf_region<Enabler, Counters...>
killable_region(perfsum_ctr<Enabler, Counters...> *ps)
{
  return killable_perf_region<Enabler, Counters...>(ps);
}


/*
 * macros for the common case of putting in a scoped perf-counting region.
 */
#define __PERF_CONCAT2(a, b)  a ## b
#define __PERF_CONCAT(a, b)   __PERF_CONCAT2(a, b)
#define __PERF_ANON	      __PERF_CONCAT(__anon_id_, __COUNTER__)

#define __PERF_REGION(region_var, sum_var, region_type, text, group)	       \
  static auto __PERF_CONCAT(sum_var, _sum) = scopedperf::perfsum(text, group); \
  auto region_var = region_type(&__PERF_CONCAT(sum_var, _sum));

#define ANON_REGION(text, group) \
  __PERF_REGION(__PERF_ANON, __PERF_ANON, scopedperf::perf_region, text, group)
#define PERF_REGION(var, text, group) \
  __PERF_REGION(var, __PERF_ANON, scopedperf::perf_region, text, group)
#define KILLABLE_REGION(var, text, group) \
  __PERF_REGION(var, __PERF_ANON, scopedperf::killable_region, text, group)

} /* namespace scopedperf */
