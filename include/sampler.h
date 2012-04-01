typedef enum { SAMP_ENABLE, SAMP_DISABLE } sampop_t;

struct sampconf {
  sampop_t op;
  u64 selector;
  u64 period;
};

#define NTRACE 4

struct pmuevent {
  u8 idle:1;
  u64 rip;
  uptr trace[NTRACE];
};

struct logheader {
  u32 ncpus;
  struct {
    u64 offset;
    u64 size;
  } cpu[];
} __attribute__((packed));
