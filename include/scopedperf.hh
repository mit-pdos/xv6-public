#pragma once

/*
 * Canonical location:
 *   git+ssh://amsterdam.csail.mit.edu/home/am1/prof/proftools.git
 *   under spmc/lib/scopedperf.hh
 */

namespace scopedperf {

typedef u32 uint;
typedef u64 uint64_t;

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
  spinlock() {
    initlock(&_lk, "sperflk", 0);
  }

  ~spinlock() {
    destroylock(&_lk);
  }

  void acquire() {
    ::acquire(&_lk);
  }

  void release() {
    ::release(&_lk);
  }

 private:
  ::spinlock _lk;
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
 * vector & pair: for portability.
 */
template<class A, class B>
struct pair {
  A first;
  B second;
};

template<class A, class B>
pair<A, B>
make_pair(const A &a, const B &b)
{
  pair<A, B> p;
  p.first = a;
  p.second = b;
  return p;
}

template<class T>
struct vector {
  T _buf[16];
  uint _cnt;

  vector() : _cnt(0) {}
  void insert_front(T e) {
    assert(_cnt < sizeof(_buf) / sizeof(T));
    memmove(&_buf[1], &_buf[0], _cnt * sizeof(T));
    _buf[0] = e;
    _cnt++;
  }
  void push_back(T e) {
    assert(_cnt < sizeof(_buf) / sizeof(T));
    _buf[_cnt] = e;
    _cnt++;
  }
};

template<class T>
struct viter {
  const vector<T> *_v;
  int _pos;

  viter(const vector<T> *v, int pos) : _v(v), _pos(pos) {}
  bool operator!=(const viter &other) const { return _pos != other._pos; }
  void operator++() { _pos++; }
  T operator*() { return _v->_buf[_pos]; }
};

template<class T>
viter<T>
begin(const vector<T> &v)
{
  return viter<T>(&v, 0);
}

template<class T>
viter<T>
end(const vector<T> &v)
{
  return viter<T>(&v, v._cnt);
}


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
  vector<const char*> get_names() const { return vector<const char*>(); }
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

  vector<const char*> get_names() const {
    vector<const char*> v = ctrgroup_chain<Others...>::get_names();
    v.insert_front(ctr->name);
    return v;
  }

  NEW_DELETE_OPS(ctrgroup_chain)

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

  perfsum_base(const char *n, display_opt d) : name(n), disp(d) {
    scoped_spinlock x(get_sums_lock());
    get_sums()->push_back(this);
  }

  static void printall(int w0 = 17, int w = 13) {
    scoped_spinlock x(get_sums_lock());
    auto sums = get_sums();
    for (perfsum_base *ps: *sums) {
      if (ps->disp == hide || !ps->get_enabled())
	continue;
      auto p = ps->get_stats();
      print_row(ps->name, ps->get_names(), w0, w, [](const char *name)
		{ return name; });
      print_row("  avg",   p, w0, w, [](const pair<uint64_t, uint64_t> &e)
	        { return e.first ? e.second / e.first : 0; });
      print_row("  total", p, w0, w, [](const pair<uint64_t, uint64_t> &e)
		{ return e.second; });
      print_row("  count", p, w0, w, [](const pair<uint64_t, uint64_t> &e)
		{ return e.first; });
    }
  }

  static void resetall() {
    scoped_spinlock x(get_sums_lock());
    for (perfsum_base *ps: *get_sums())
      ps->reset();
  }

  virtual vector<pair<uint64_t, uint64_t> > get_stats() const = 0;
  virtual vector<const char*> get_names() const = 0;
  virtual bool get_enabled() const = 0;
  virtual void reset() = 0;

 private:
  template<class Row, class Callback>
  static void print_row(const char *rowname, const Row &r,
			int w0, int w, Callback f)
  {
    std::cout << std::left << std::setw(w0) << rowname;
    for (const auto &elem: r)
      std::cout << std::left << std::setw(w) << f(elem) << " ";
    std::cout << std::endl;
  }

  static vector<perfsum_base*> *get_sums() {
    static vector<perfsum_base*> v;
    return &v;
  }

  static spinlock *get_sums_lock() {
    static spinlock l;
    return &l;
  }

  const char *name;
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
	      const char *n, display_opt d)
    : perfsum_base(n, d), cg(c), base(0)
  {
    reset();
  }

  perfsum_ctr(const char *n,
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

  vector<pair<uint64_t, uint64_t> > get_stats() const /* override */ {
    vector<pair<uint64_t, uint64_t> > v;
    for (uint i = 0; i < cg->nctr; i++) {
      uint64_t b =
	base ? base->addcpus([i](const stats *s) { return s->sum[i]; })
	     : addcpus([](const stats *s) { return s->count; });
      v.push_back(make_pair(b,
	addcpus([i](const stats *s) { return s->sum[i]; })));
    }
    return v;
  }

  vector<const char*> get_names() const /* override */ {
    return cg->get_names();
  }

  bool get_enabled() const /* override */ {
    return Enabler::enabled();
  }

  void reset() /* override */ {
    memset(stat, 0, sizeof(stat));
  }

  NEW_DELETE_OPS(perfsum_ctr)

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
  perfsum_ctr_inlinegroup(const char *n, perfsum_base::display_opt d,
                          Counters*... ctrs)
    : ctrgroup_chain<Counters...>(ctrs...),
      perfsum_ctr<Enabler, Counters...>(this, n, d) {}
};

template<typename Enabler = default_enabler, typename... Counters>
perfsum_ctr<Enabler, Counters...>
perfsum(const char *name, const ctrgroup_chain<Counters...> *c,
	const perfsum_base::display_opt d = perfsum_base::show)
{
  return perfsum_ctr<Enabler, Counters...>(c, name, d);
}

template<typename Enabler = default_enabler, typename... Counters>
perfsum_ctr_inlinegroup<Enabler, Counters...>
perfsum_group(const char *name, Counters*... c)
{
  return perfsum_ctr_inlinegroup<Enabler, Counters...>(name, perfsum_base::show, c...);
}

template<typename Enabler, typename... Counters>
perfsum_ctr<Enabler, Counters...>
perfsum_frac(const char *name,
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
  namedctr(const char *n) : name(n) {}
  void setup() {}
  const char *name;
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
  NEW_DELETE_OPS(tsc_ctr)
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
  pmc_ctr(const char *nm) : namedctr<CounterWidth>(nm), cn(-1) {}

  uint64_t sample() const {
    uint64_t a, d;
    __asm __volatile("rdpmc" : "=a" (a), "=d" (d) : "c" (cn));
    return a | (d << 32);
  }

  int cn;

 private:
  static const char* mkname(int n) {
    char *buf = new char[32];
    snprintf(buf, 32, "pmc%d", n);
    return buf;
  }
};

template<uint64_t CounterWidth = 64>
class pmc_setup : public pmc_ctr<CounterWidth> {
 public:
  pmc_setup(uint64_t v, const char *nm)
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

#if 0
    // ugly but effective
    std::stringstream ss;
    ss << "for f in /sys/kernel/spmc/cpu*/" << n << "; do "
       << "echo " << std::hex << pmc_v << " > $f; done";
    assert(0 == system(ss.str().c_str()));
#endif

    pmc_ctr<CounterWidth>::cn = n;
  }

 private:
  uint64_t pmc_v;
};

#if 0
class tod_ctr : public namedctr<64> {
 public:
  tod_ctr() : namedctr("tod-usec") {}
  uint64_t sample() const {
    struct timeval tv;
    gettimeofday(&tv, 0);
    return ((uint64_t) tv.tv_usec) + ((uint64_t) tv.tv_sec) * 1000000;
  }
};
#endif

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
    : ps(psarg), enabled(ps->enabled()), cpuid(enabled ? mycpu()->id : 0)
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
