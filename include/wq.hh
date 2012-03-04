struct work {
  void *rip;
  void *arg0;
  void *arg1;
  void *arg2;
  void *arg3;
  void *arg4;
  char data[];
};

void            initwq(void);
struct work *   allocwork(void);
void            freework(struct work *w);
int             wq_push(struct work *w);

template <typename IT, typename BODY>
static inline void
wq_for(IT &init, bool (*cond)(IT &it), BODY body)
{
  // XXX(sbw) should be able to coarsen loop
  for (IT &it = init; cond(it); ++it) {
    body(*it);
  }
}
