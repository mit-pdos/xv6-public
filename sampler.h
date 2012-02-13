typedef enum { SAMP_ENABLE, SAMP_DISABLE } sampop_t;

struct sampconf {
  sampop_t op;
  u64 selector;
  u64 period;
};

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
