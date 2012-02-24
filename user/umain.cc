#include <unistd.h>
#include <signal.h>
#include <getopt.h>

#include "crange_arch.hh"
#include "gc.hh"
#include "crange.hh"
#include "atomic_util.hh"
#include "ns.hh"
#include "scopedperf.hh"
#include "intelctr.hh"
#include "arc4.hh"
#include "amd64.h"

static auto perfgroup = ctrgroup(&intelctr::tsc
                                // ,&intelctr::l2_refs
                                // ,&intelctr::l2_miss
                                );

u64
proc_hash(const u32 &pid)
{
  return pid;
}

pthread_key_t myproc_key, arc4_key;
cpu cpus[NCPU];
u32 ncpu;
u64 ticks;
xns<u32, proc*, proc_hash> *xnspid;

static auto rnd_perfsum = scopedperf::perfsum("arc4 rnd", &perfgroup);

template<class T>
T rnd()
{
  auto __PERF_ANON = scopedperf::perf_region(&rnd_perfsum);

  arc4 *a = (arc4*) pthread_getspecific(arc4_key);
  if (!a) {
    struct seed { u64 a, b; } s = { rdtsc(), pthread_self() };
    a = new arc4((u8*) &s, sizeof(s));
    pthread_setspecific(arc4_key, a);
  }

  return a->rand<T>();
}

static void*
proc_start(void *arg)
{
  proc *p = (proc *) arg;

  pthread_setspecific(myproc_key, p);
  p->pid = pthread_self();
  initprocgc(p);
  xnspid->insert(p->pid, p);

  p->f(p->farg);
  return 0;
}

void
makeproc(proc *p)
{
  pthread_t tid;
  pthread_create(&tid, 0, &proc_start, p);
}

void
threadpin(void (*fn)(void*), void *arg, const char *name, int cpu)
{
  proc *p = new proc();
  memset(p, 0, sizeof(*p));
  p->f = fn;
  p->farg = arg;
  snprintf(p->name, sizeof(p->name), "%s", name);
  p->cpuid = cpu;

  makeproc(p);
}

struct my_range : public range {
  my_range(crange *cr, u64 k, u64 sz) : range(cr, k, sz) {}
  virtual void do_gc() { delete this; }
};

static pthread_barrier_t worker_b, populate_b;

enum { iter_total = 1000000 };
enum { crange_items = 1024 };

static void
worker(void *arg)
{
  crange *cr = (crange*) arg;

  for (u32 i = 0; i < iter_total / ncpu; i++) {
    ANON_REGION("worker op", &perfgroup);
    u64 k = 1 + rnd<u32>() % (crange_items * 2);
    auto span = cr->search_lock(k, 1);
    if (rnd<u8>() & 1) {
      ANON_REGION("worker del", &perfgroup);
      span.replace(0);
    } else {
      ANON_REGION("worker add", &perfgroup);
      span.replace(new my_range(cr, k, 1));
    }
  }

  pthread_barrier_wait(&worker_b);
}

static void
populate(void *arg)
{
  crange *cr = (crange*) arg;
  for (u32 i = 0; i < crange_items; i++)
    cr->search_lock(1 + 2*i, 1).replace(new my_range(cr, 1+2*i, 1));
  pthread_barrier_wait(&populate_b);
}

static const struct option long_opts[] = {
  { "ncpu", required_argument, 0, 'n' },
  { 0, no_argument, 0, 0 }
};

static u32
l2(u64 v)
{
  u32 l = 0;

  while (v) {
    v = v>>1;
    l++;
  }

  return l;
}

int
main(int ac, char **av)
{
  ncpu = NCPU;

  for (;;) {
    int long_idx;
    int opt = getopt_long(ac, av, "n:", long_opts, &long_idx);
    if (opt == -1)
      break;

    switch (opt) {
    case 'n':
      ncpu = atoi(optarg);
      assert(ncpu <= NCPU);
      break;

    case '?':
      printf("Options:\n");
      for (u32 i = 0; long_opts[i].name; i++)
        printf("  -%c / --%s%s\n",
               long_opts[i].val,
               long_opts[i].name,
               long_opts[i].has_arg == required_argument ? " ARG" :
               long_opts[i].has_arg == optional_argument ? " [ARG]" : "");
      exit(-1);
    }
  }

  assert(0 == pthread_key_create(&myproc_key, 0));
  assert(0 == pthread_key_create(&arc4_key, 0));
  for (u32 i = 0; i < NCPU; i++)
    cpus[i].id = i;

  xnspid = new xns<u32, proc*, proc_hash>(false);
  initgc();

  pthread_barrier_init(&populate_b, 0, 2);
  crange cr(l2(crange_items));
  threadpin(populate, &cr, "populate", 0);
  pthread_barrier_wait(&populate_b);

  pthread_barrier_init(&worker_b, 0, ncpu+1);
  for (u32 i = 0; i < ncpu; i++) {
    char buf[32];
    sprintf(buf, "worker%d", i);
    threadpin(worker, &cr, buf, i);
  }
  pthread_barrier_wait(&worker_b);

  scopedperf::perfsum_base::printall();
}
