struct padded_length {
  volatile u64 v_ __mpalign__;;
  __padout__;
};

#if defined (XV6_KERNEL)
struct uwq_length {
  uwq_length(padded_length *len) : len_(len) {}

  void clear() {
    for (int i = 0; i < NCPU; i++)
      len_[i].v_ = 0;
  }

  bool haswork() {
    for (int i = 0; i < NCPU; i++) {
      if (len_[i].v_ > 0)
        return true;
    }
    return false;
  }

private:
  padded_length* len_;
};

int             uwq_trywork(void);
#endif
