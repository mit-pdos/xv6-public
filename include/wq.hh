struct work {
  void *rip;
  void *arg0;
  void *arg1;
  void *arg2;
  void *arg3;
  void *arg4;
  char data[];
};

struct run_work {
  virtual void run() = 0;
};

void            initwq(void);
struct work *   allocwork(void);
void            freework(struct work *w);
int             wq_push(struct work *w);

template<typename IT, typename BODY>
struct for_work : public run_work {
  for_work(IT &it, BODY body) : it_(it), body_(body) {}

  virtual void run() { printf("hi %s\n", *it_); }

  IT &it_;
  bool (*cond_)(IT &it);
  BODY &body_;
};

static inline void
wq_push_cpp(struct run_work *w)
{
  w->run();
}

template <typename IT, typename BODY>
static void
wq_for_one(struct work *w, void *a0, void *a1, void *a2)
{
  
}

template <typename IT, typename BODY>
static inline void
wq_for(IT &init, bool (*cond)(IT &it), BODY body)
{
  for_work<IT, BODY> goo(init, body);

  wq_push_cpp(&goo);

#if 0
  BODY foo = body;

  // XXX(sbw) should be able to coarsen loop
  for (IT &it = init; cond(it); ++it) {
    foo(*it);
    body(*it);
  }
#endif
}
