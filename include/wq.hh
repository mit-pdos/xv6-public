#pragma once

#if defined(LINUX)
#include <stdlib.h>
#include <assert.h>
#include <atomic>
#define xmalloc(n)     malloc(n)
#define xfree(p, sz)   free(p)
#elif defined(XV6_KERNEL)
#define xmalloc(n)   kmalloc(n) 
#define xfree(p, sz) kmfree(p, sz)
#else
#warning "Unknown wq implementation"
#endif

#define WQSIZE 8192

class work;

int             wq_trywork(void);
int             wq_push(work *w);
void            initwq(void);

struct work {
  virtual void run() = 0;
};

struct cwork : public work {
  virtual void run();

  static void* operator new(unsigned long);
  static void* operator new(unsigned long, cwork*);
  static void operator delete(void*p);

  void *rip;
  void *arg0;
  void *arg1;
  void *arg2;
  void *arg3;
  void *arg4;
};

typedef std::atomic<int> forframe_t;

template <typename IT, typename BODY>
struct forwork : public work {
  forwork(IT &it, bool (*cond)(IT &it), BODY &body, forframe_t &frame) 
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
  forframe_t &frame_;
};

template <typename IT, typename BODY>
static inline void
wq_for(IT &init, bool (*cond)(IT &it), BODY body)
{
  forframe_t frame(0);

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
