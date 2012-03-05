template <typename IT, typename BODY>
struct forwork : public work {
  typedef std::atomic<int> forframe;

  forwork(IT &it, bool (*cond)(IT &it), BODY &body, forframe &frame) 
    : it_(it), cond_(cond), body_(body), frame_(frame) {}

  virtual void run() {
    decltype(*it_) v = *it_;
    ++it_;
    if (cond_(it_)) {
      forwork<IT, BODY> *w = new forwork<IT, BODY>(it_, cond_, body_, frame_);
      ++frame_;
      wq_push(w);    
    }
    body_(v);
    --frame_;
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
  typename forwork<IT, BODY>::forframe frame(0);

  // XXX(sbw) should be able to coarsen loop

  decltype(*init) v = *init;
  ++init;
  if (cond(init)) {
    forwork<IT, BODY> *w = new forwork<IT, BODY>(init, cond, body, frame);
    ++frame;
    wq_push(w);
  }
  body(v);

  while (frame != 0)
    wq_trywork();
}
