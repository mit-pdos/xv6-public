#define WQSIZE 8192
class work;

int             wq_trywork(void);
int             wq_push(work *w);
void            initwq(void);

struct work {
  virtual void run() = 0;
};

struct cwork : public work{
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

template <typename IT, typename BODY>
static inline void
wq_for(IT &init, bool (*cond)(IT &it), BODY body)
{
  // XXX(sbw) should be able to coarsen loop
  for (IT &it = init; cond(it); ++it) {
    body(*it);
  }
}
