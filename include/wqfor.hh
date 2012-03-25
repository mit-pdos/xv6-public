struct forframe
{
  forframe(int v) : v_(v) {}
  int inc() { return __sync_add_and_fetch(&v_, 1); }
  int dec() { return __sync_sub_and_fetch(&v_, 1); }
  bool zero() volatile { return v_ == 0; };
  volatile int v_;
};

template <typename IT, typename BODY>
struct forwork : public work {
  forwork(IT &it, bool (*cond)(IT &it), BODY &body, forframe &frame) 
    : it_(it), cond_(cond), body_(body), frame_(frame) {}

  virtual void run() {
    decltype(copy_value(it_)) v = copy_value(it_);
    ++it_;
    if (cond_(it_)) {
      forwork<IT, BODY> *w = new forwork<IT, BODY>(it_, cond_, body_, frame_);
      frame_.inc();
      wq_push(w);    
    }
    body_(v);
    frame_.dec();
    delete this;
  }

  static void* operator new(unsigned long nbytes) {
    assert(nbytes == sizeof(forwork<IT, BODY>));
    return xmalloc(sizeof(forwork));
  }

  static void operator delete(void*p) {
    xfree(p, sizeof(forwork));
  }

  IT &it_;
  bool (*cond_)(IT&);
  BODY &body_;
  forframe &frame_;
};

template <typename IT, typename BODY>
static inline void
wq_for(IT &init, bool (*cond)(IT &it), BODY body)
{
  forframe frame(0);

  // XXX(sbw) should be able to coarsen loop

  if (!cond(init))
    return;

  decltype(copy_value(init)) v = copy_value(init);
  ++init;
  if (cond(init)) {
    forwork<IT, BODY> *w = new forwork<IT, BODY>(init, cond, body, frame);
    frame.inc();
    wq_push(w);
  }

  body(v);

  while (!frame.zero())
    wq_trywork();
}

template <typename IT, typename BODY>
static inline void
wq_for_serial(IT &init, bool (*cond)(IT &it), BODY body)
{
  for (; cond(init); ++init)
    body(copy_value(init));
}

template <typename T>
static inline T
copy_value(const T &it)
{
  return it;
}
