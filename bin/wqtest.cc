#include "types.h"
#include "user.h"
#include "lib.h"
#include "amd64.h"
#include "wq.hh"

#define NEW_DELETE_OPS(classname)                                   \
  static void* operator new(unsigned long nbytes) {                 \
    assert(nbytes == sizeof(classname));                            \
    return malloc(sizeof(classname));                               \
  }                                                                 \
                                                                    \
  static void operator delete(void *p) {                            \
    free(p);                                                        \
  }

struct testwork : public work {
  testwork(wframe *b) : barrier_(b) {}

  virtual void run() {
    barrier_->dec();
    delete this;
  }

  NEW_DELETE_OPS(testwork);
  struct wframe *barrier_;
};

static void
test0(void)
{
  enum { pushes = 100 };
  struct wframe wqbarrier(pushes);

  printf("test0...\n");
  for (int i = 0; i < pushes; i++) {
    testwork *w = new testwork(&wqbarrier);
    wq_push(w);
  }

  while (!wqbarrier.zero())
    nop_pause();
  printf("test0 done\n");
}

struct forkwork : public work {
  forkwork(wframe *b) : barrier_(b) {}

  virtual void run() {
    int pid;

    pid = fork(0);
    if (pid < 0)
      die("forkwork::run: fork");
    else if (pid == 0)
      exit();
    wait();

    barrier_->dec();
    delete this;
  }

  NEW_DELETE_OPS(forkwork);
  struct wframe *barrier_;
};

static void
testfork(void)
{
  enum { forks = 100 };
  struct wframe wqbarrier(forks);

  printf("testfork...\n");
  for (int i = 0; i < forks; i++) {
    forkwork *w = new forkwork(&wqbarrier);
    wq_push(w);
  }

  while (!wqbarrier.zero())
    nop_pause();
  printf("testfork done\n");
}

struct execwork : public work {
  execwork(wframe *b) : barrier_(b) {}

  virtual void run() {
    int pid;

    pid = fork(0);
    if (pid < 0)
      die("execwork::run: fork");
    else if (pid == 0) {
      static const char *args[] = { "echo", 0 };
      exec(args[0], args);
      die("execwork: exec failed");
    }
    wait();

    barrier_->dec();
    delete this;
  }

  static void test(void) {
    enum { execs = 100 };
    struct wframe wqbarrier(execs);

    printf("testexec...\n");
    for (int i = 0; i < execs; i++) {
      execwork *w = new execwork(&wqbarrier);
      wq_push(w);
    }
    
    while (!wqbarrier.zero())
      nop_pause();
    printf("testexec done\n");
  }

  NEW_DELETE_OPS(execwork);
  struct wframe *barrier_;
};

int
main(int ac, char **av)
{
  initwq();
  test0();
  testfork();
  execwork::test();
  return 0;
}
