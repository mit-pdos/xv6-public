#include <unistd.h>

#include "crange_arch.hh"
#include "gc.hh"
#include "crange.hh"
#include "atomic_util.hh"
#include "ns.hh"

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

struct makeproc_info {
  void (*f) (void*);
  void *farg;
  const char *name;
  int cpu;
};

void
cli()
{
  /* suspend all threads with the same cpuid */
}

void
sti()
{
  /* resume all threads with the same cpuid */
}

static void*
makeproc_start(void *arg)
{
  makeproc_info *mpi = (makeproc_info *) arg;

  proc *p = new proc();
  pthread_setspecific(myproc_key, p);
  p->pid = pthread_self();
  p->cpuid = mpi->cpu;
  snprintf(p->name, sizeof(p->name), "%s", mpi->name);
  initprocgc(p);
  xnspid->insert(p->pid, p);

  mpi->f(mpi->farg);
  delete mpi;
  return 0;
}

void
makeproc(makeproc_info *mpi)
{
  pthread_t tid;
  makeproc_info *mcopy = new makeproc_info(*mpi);
  pthread_create(&tid, 0, &makeproc_start, mcopy);
}

void
threadpin(void (*fn)(void*), void *arg, const char *name, int cpu)
{
  makeproc_info mpi = { fn, arg, name, cpu };
  makeproc(&mpi);
}

int
main(int ac, char **av)
{
  assert(0 == pthread_key_create(&myproc_key, 0));
  for (u32 i = 0; i < NCPU; i++)
    cpus[i].id = i;
  ncpu = NCPU;
  xnspid = new xns<u32, proc*, proc_hash>(false);

  initgc();

  printf("Hello world!\n");
  sleep(100);
}
