#include <unistd.h>
#include <signal.h>
#include <getopt.h>

#include "crange_arch.hh"
#include "gc.hh"
#include "crange.hh"
#include "atomic_util.hh"
#include "ns.hh"
#include "rnd.hh"

u64
proc_hash(const u32 &pid)
{
  return pid;
}

pthread_key_t myproc_key;
cpu cpus[NCPU];
u32 ncpu;
u64 ticks;
xns<u32, proc*, proc_hash> *xnspid;

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

static pthread_barrier_t worker_b, populate_b;

enum { iter_total = 10000000 };
enum { crange_items = 1024 };

static void
worker(void *arg)
{
  crange *cr = (crange*) arg;

  for (u32 i = 0; i < iter_total / ncpu; i++) {
    u64 k = 1 + rnd() % (crange_items * 2);
    auto span = cr->search_lock(k, 1);
    if (rnd() & 1)
      span.replace(0);
    else
      span.replace(new range(cr, k, 1));
  }

  pthread_barrier_wait(&worker_b);
}

static void
populate(void *arg)
{
  crange *cr = (crange*) arg;
  for (u32 i = 0; i < crange_items; i++)
    cr->search_lock(1 + 2*i, 1).replace(new range(cr, 1+2*i, 1));
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

  printf("exiting\n");
}
