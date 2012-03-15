#pragma once

struct padded_length {
  volatile u64 v_ __mpalign__;;
  __padout__;
};

#if defined (XV6_KERNEL)
struct uwq {
  uwq(padded_length *len);
  ~uwq();
  bool haswork();
  void* buffer();

private:
  uwq(const uwq& x);
  padded_length* len_;
};

int             uwq_trywork(void);
#endif
