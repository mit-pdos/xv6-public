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
