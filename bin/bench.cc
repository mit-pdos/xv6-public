#include "types.h"
#include "user.h"
#include "amd64.h"

static u64
time_this(const char *av[])
{
  u64 t0 = rdtsc();

  int pid = fork(0);
  if (pid < 0)
    die("time_this: fork failed %s", av[0]);

  if (pid == 0) {
    exec(av[0], av);
    die("time_this: exec failed %s", av[0]);
  }

  wait();
  u64 t1 = rdtsc();
  return t1-t0;
}

struct Bench
{
  virtual void run(void) = 0;
  virtual char* result(void) = 0;  
};

#define NEW_OPERATOR(classname)                                         \
  void* operator new(unsigned long nbytes) {                            \
    assert(nbytes == sizeof(classname));                                \
    return malloc(sizeof(classname));                                   \
  }

struct TimedExec : public Bench
{
  TimedExec(const char **argv) : argv_(argv) {}
  
  virtual void run(void) {
    char *str = result_;
    *str = 0;
    for (int i = 0; argv_[i]; i++) {
      int n = sizeof(result_)-(str-result_);
      snprintf(str, n, i==0?"%s":" %s", argv_[i]);
      str += strlen(str);
    }

    printf("%s .. ", result_);
    u64 r = time_this(argv_);
    snprintf(str, sizeof(result_)-(str-result_), ": %lu cycles", r);
    printf("%lu cycles\n", r);
  }

  virtual char* result(void) {
    return result_;
  }

  const char **argv_;
  char result_[256];

  NEW_OPERATOR(TimedExec)
};

struct LoopsBench : public Bench
{
  LoopsBench(const char *cmd, int nloops, int cpuinc) :
    Bench(), cmd_(cmd), nloops_(nloops), cpuinc_(cpuinc) {}

  char* runone(int ncore, const char** argv, char *res, int n) {
      char cores[16];
      snprintf(cores, sizeof(cores), "%u", ncore);
      argv[1] = cores;
      u64 r = time_this(argv);      
      // r in usecs
      r = (r*(1000*1000)) / cpuhz();
      snprintf(res, n, "%u %lu %lu\n", ncore, r, nloops_);
      return res + strlen(res);
  }

  virtual void run(void) {
    char nloopstr[16];
    snprintf(nloopstr, sizeof(nloopstr), "%u", nloops_); 
    const char *argv[] = { cmd_, 0, nloopstr, 0 };

    char *res = result_;
    char *q = res + sizeof(result_);

    snprintf(res, q-res, "#cores usecs loops/core\n");
    res += strlen(res);

    res = runone(1, argv, res, q-res);
    for (int i = cpuinc_; i <= NCPU; i += cpuinc_)
      res = runone(i, argv, res, q-res);
  }

  virtual char* result(void) {
    return result_;
  }

  const char* cmd_;
  int nloops_;
  int cpuinc_;
  char result_[1024];
  NEW_OPERATOR(LoopsBench)
};

#define CMD(...) new TimedExec((const char *[]){__VA_ARGS__, 0})
#define STR_1(x...) #x
#define STR(x...)   STR_1(x)

int
main(int ac, char **av)
{
  static Bench* the_bench[128];
  int n = 0;

  //the_bench[n++] = new LoopsBench("/dirbench", 1000, 6);
  //the_bench[n++] = new LoopsBench("/filebench", 100000, 6);
  the_bench[n++] = new LoopsBench("/mapbench", 10000, 6);

  //the_bench[n++] = CMD("filebench", "6", "10000");
  //the_bench[n++] = CMD("filebench", "48", "10000");

  //the_bench[n++] = CMD("forkexecbench");
  //the_bench[n++] = CMD("mktree", STR(NCPU), "tree.xdu", "4", "4");
  //the_bench[n++] = CMD("xdu", "8");
  //the_bench[n++] = CMD("xdu", "4");
  //the_bench[n++] = CMD("xdu", "1");
  //the_bench[n++] = CMD("xls", "8");
  //the_bench[n++] = CMD("xls", "4");
  //the_bench[n++] = CMD("xls", "1");

  //the_bench[n++] = CMD("mapbench", "1");
  //the_bench[n++] = CMD("mapbench", "2");
  //the_bench[n++] = CMD("mapbench", "16");
  
  for (int i = 0; i < n; i++)
    the_bench[i]->run();

  printf("\n");
  for (int i = 0; i < n; i++)
    printf("%s\n", the_bench[i]->result());
}
