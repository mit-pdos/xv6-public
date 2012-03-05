#if defined(XV6_KERNEL)
typedef struct spinlock wqlock_t;
#elif defined(LINUX)
typedef pthread_spinlock_t wqlock_t;
#else
typedef struct uspinlock wqlock_t;
#endif

#include "percpu.hh"

#define NSLOTS (1 << WQSHIFT)

struct work {
  virtual void run() = 0;
};

class wq {
public:
  wq();
  int push(work *w);
  int trywork();
  void dump();

  static void* operator new(unsigned long);

private:
  work *steal(int c);
  work *pop(int c);

  struct wqueue {
    work *w[NSLOTS];
    volatile int head __mpalign__;
    volatile int tail;
    wqlock_t lock;
  };

  struct stat {
    u64 push;
    u64 full;
    u64 pop;
    u64 steal;
  };

  percpu<wqueue> q_;
  percpu<stat> stat_;
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

void            initwq(void);
struct work *   allocwork(void);
void            freework(struct work *w);
int             wq_push(work *w);

template <typename IT, typename BODY>
static inline void
wq_for(IT &init, bool (*cond)(IT &it), BODY body)
{
  // XXX(sbw) should be able to coarsen loop
  for (IT &it = init; cond(it); ++it) {
    body(*it);
  }
}
