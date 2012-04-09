#include "sched.hh"

struct pmc_count {

  static void config(u64 sel) {
    perf_start(sel, 0);
  }

  static pmc_count read(unsigned int ctr) {
    pmc_count n;

    // XXX(sbw) we loose our original affinity
    for (int i = 0; i < NCPU; i++) {
      setaffinity(i);
      // XXX(sbw) qemu doesn't seem to support CR4_PCE
#if defined(HW_qemu)
      n.count_[i] = 0;
#else
      n.count_[i] = rdpmc(ctr);
#endif
    }
    setaffinity(-1);
    return n;
  }

  pmc_count delta(const pmc_count& o) {
    pmc_count n;

    for (int i = 0; i < NCPU; i++)
      n.count_[i] = count_[i] - o.count_[i];
    return n;
  }

  u64 sum() const {
    u64 tot = 0;

    for (int i = 0; i < NCPU; i++)
      tot += count_[i];
    return tot;
  }

  u64 count_[NCPU];
};

struct sys_stat {
  static sys_stat* read() {
    sys_stat* that;
    int fd;
    long r;

    that = new sys_stat();
    assert(that != nullptr);
    fd = open("/dev/stat", O_RDONLY);
    assert(fd != -1);
    r = ::read(fd, that->stats, sizeof(that->stats));
    assert(r == sizeof(that->stats));
    
    return that;
  }

  sys_stat* delta(const sys_stat* o) const {
    sys_stat* that;

    that = new sys_stat();    
    for (int i = 0; i < NCPU; i++) {
      that->stats[i].enqs = stats[i].enqs - o->stats[i].enqs;
      that->stats[i].deqs = stats[i].deqs - o->stats[i].deqs;
      that->stats[i].steals = stats[i].steals - o->stats[i].steals;
      that->stats[i].misses = stats[i].misses - o->stats[i].misses;
      that->stats[i].idle = stats[i].idle - o->stats[i].idle;
      that->stats[i].busy = stats[i].busy - o->stats[i].busy;
    }

    return that;
  }

  u64 busy() const {
    u64 tot = 0;
    for (int i = 0; i < NCPU; i++)
      tot += stats[i].busy;
    return tot;
  }

  u64 idle() const {
    u64 tot = 0;
    for (int i = 0; i < NCPU; i++)
      tot += stats[i].idle;
    return tot;
  }

  static void* operator new(unsigned long nbytes) {
    assert(nbytes == sizeof(sys_stat));
    return malloc(nbytes);
  }

  static void operator delete(void* p) {
    free(p);
  }

  sched_stat stats[NCPU];
};
