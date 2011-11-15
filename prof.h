struct profrec {
  u64 tot;
  u64 cnt;
  __padout__;
} __mpalign__;

typedef struct profctr {
  const char *name;
  struct profrec rec[NCPU] __mpalign__;
  __padout__;
} profctr_t;

#define DEFINE_PROFCTR(xname) \
  profctr_t xname __attribute__((section(".prof"))) = { .name = #xname };

#define prof_start(name) \
  u64 __prof##name = profenable ? rdtsc() : 0;

#define prof_end(name) do { \
  if (profenable) { \
    u64 __eprof##name = rdtsc(); \
    u64 __profid = mycpu()->id; \
    name.rec[__profid].tot += __eprof##name - __prof##name; \
    name.rec[__profid].cnt++; \
  } \
} while (0)
