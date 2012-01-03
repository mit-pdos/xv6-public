struct pmuevent {
  u64 rip;
};

struct logheader {
  u32 ncpus;
  struct {
    u64 offset;
    u64 size;
  } cpu[];
};
