#include "types.h"
#include "user.h"
#include "lib.h"
#include "amd64.h"
#include "wq.hh"

struct testwork : public work {
  testwork(forframe *b) : barrier_(b) {}

  virtual void run() {
    barrier_->dec();
    delete this;
  }

  static void* operator new(unsigned long nbytes) {
    assert(nbytes == sizeof(testwork));
    return xmalloc(sizeof(testwork));
  }

  static void operator delete(void*p) {
    xfree(p, sizeof(testwork));
  }

  struct forframe *barrier_;
};

static void
test(void)
{
  enum { pushes = 100 };
  struct forframe wqbarrier(pushes);
  
  for (int i = 0; i < pushes; i++) {
    testwork *w = new testwork(&wqbarrier);
    wq_push(w);
  }

  while (!wqbarrier.zero())
    nop_pause();
}

int
main(int ac, char **av)
{
  initwq();
  sleep(5);
  test();
  for (;;);
  exitwq();
  printf("all done!\n");
  return 0;
}
