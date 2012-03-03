struct work {
  void *rip;
  void *arg0;
  void *arg1;
  void *arg2;
  void *arg3;
  void *arg4;
  char data[];
};

template <typename IT, typename BODY>
static inline void
wq_for(IT &init, bool (*cond)(IT &it), BODY body) {
  for (IT &it = init; cond(it); ++it) {
    body(it);
  }
}

