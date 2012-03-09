//
// A simple shell that uses a dependency-aware layer on
// top of wq to schedule commands.
//
// If you give this script to wqsh:
//   cp README x
//   cp x y
//   cp README z
// It will ask wq to run the two "cp README ..." commands in parallel,
// but will block the "cp x y" until the first cp has completed.
//

#include "types.h"
#include "user.h"
#include "fcntl.h"
#include "wq.hh"

//
// Adapt wq to understand simple dependencies
//
class dwork;
class dwq {
private:
  static const int max = 100;

  // keep track of sequence of push()ed dworks.
  // so we know which are done, and thus what
  // dependencies are or are not satisfied.
  int n;
  dwork *da[max];

  // is it OK to run da[i]?
  int can_run(int i);

public:
  static void * operator new(unsigned long n){
    return malloc(n);
  }
  dwq() { n = 0; }
  int done();
  void push(dwork *);
  void run();
};

class dwork {
private:
  friend class dwq;
  cwork *w;
  int started;
  int done;
  static const int max = 10;
  int nneeds, nsatisfies;
  int _needs[max];
  int _satisfies[max];
  static void gogo(void *a0, void *a1, void *a2);
public:

  static void * operator new(unsigned long n){
    return malloc(n);
  }
  dwork(void *fn, void *arg);
  void needs(int); // must wait for this dependency, for input
  void satisfies(int); // output satisfies this dependency
};

void
dwq::push(dwork *dw)
{
  assert(n+1 < max);
  da[n++] = dw;
}

int
dwq::done()
{
  int ndone = 0;
  for(int i = 0; i < n; i++)
    if(da[i]->done)
      ndone++;
  return ndone >= n;
}

int
dwq::can_run(int di)
{
  dwork *w = da[di];
  int ready[dwork::max];

  // by default, all symbolic resources are ready.
  for(int i = 0; i < w->nneeds; i++)
    ready[i] = 1;

  // for each input w needs, figure out if the most
  // recent provider is done.
  for(int i = 0; i < di; i++){
    for(int j = 0; j < w->nneeds; j++){
      for(int k = 0; k < da[i]->nsatisfies; k++){
        if(w->_needs[j] == da[i]->_satisfies[k]){
          ready[j] = da[i]->done;
        }
      }
    }
  }

  // are all inputs ready?
  for(int i = 0; i < w->nneeds; i++){
    if(ready[i] == 0){
      printf("can_run(%d) 0\n", di);
      return 0;
    }
  }

  printf("can_run(%d) 1\n", di);
  return 1;
}

// find dworks w/ satisfied inputs, run them, see
// if they un-block further dworks, &c.
void
dwq::run()
{
  while(done() == 0){
    for(int i = 0; i < n; i++){
      if(da[i]->done == 0 && da[i]->started == 0 &&
         can_run(i)){
        da[i]->started = 1;
        wq_push(da[i]->w);
      }
    }
    wq_trywork();
  }
}

void
dwork::gogo(void *a0, void *a1, void *a2)
{
  dwork *w = (dwork *) a0;
  auto fn = (void (*)(void *)) a1;
  fn(a2);
  w->done = 1;
}

dwork::dwork(void *fn, void *arg)
{
  w = new cwork();
  w->rip = (void *) dwork::gogo;
  w->arg0 = this;
  w->arg1 = fn;
  w->arg2 = arg;
  started = 0;
  done = 0;
  nneeds = 0;
  nsatisfies = 0;
}

// this dwork must wait for some previously pushed
// dwork to generate output with symbolic name x.
void
dwork::needs(int x)
{
  assert(nneeds < max);
  _needs[nneeds++] = x;
}

// this dwork generates output with symbolic name x,
// so any dwork needing (waiting for) x can start.
void
dwork::satisfies(int x)
{
  assert(nsatisfies < max);
  _satisfies[nsatisfies++] = x;
}

//
// parse shell scripts
//
class thing {
protected:
  int done;
public:
  static void * operator new(unsigned long n){
    return malloc(n);
  }
  static void operator delete(void *p){
    free(p);
  }
  thing() { done = 0; }
  virtual ~thing();
  virtual void print() = 0;
  virtual void run(dwq *) = 0;
  int checkdone() { return done; }
};

class cmd : public thing {
private:
  int argc;
  char *argv[20];
public:
  cmd() { argc = 0; }

  void print();
  void run(dwq *);
  void reallyrun();
  static cmd *parse();
};

class sequence : public thing {
private:
  int n;
  class cmd *a[20];
public:
  sequence() { n = 0; }

  void print();
  void run(dwq *);
  static sequence *parse();
};

thing::~thing()
{
}

int
isspace(char c)
{
  return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

cmd *
cmd::parse()
{
  char buf[512];
  gets(buf, sizeof(buf));
  if(buf[0] == 0)
    return 0;

  cmd *c = new cmd();
  for(int i = 0; buf[i]; ){
    int j;
    for(j = i; buf[j] && !isspace(buf[j]); j++)
      ;

    char *p = (char *) malloc(j - i + 1);
    assert(p);
    memmove(p, buf + i, j - i);
    p[j - i] = '\0';
    assert(c->argc < 20);
    c->argv[c->argc++] = p;

    while(isspace(buf[j]))
      j++;
    i = j;
  }
  return c;
}

void
blah(void *x)
{
  cmd *c = (cmd *) x;
  c->reallyrun();
}

void
cmd::reallyrun()
{
  int pid = fork(0);
  if(pid < 0){
    printf("fork failed\n");
    exit();
  }
  if(pid == 0){
    argv[argc] = 0;
    exec(argv[0], (const char **) argv);
    printf("exec failed\n");
    exit();
  } else {
    wait();
    done = 1;
  }
}

void
cmd::run(dwq *d)
{
  dwork *w = new dwork((void *) blah, this);
  if(argc == 3 && strcmp(argv[0], "cp") == 0){
    // this isn't correct; it's pessimistic in that
    // distinct files can be mapped to the same "need" key,
    // and that if you use different names for the same
    // file, we won't realize they are the same.
    w->needs(argv[1][0]);
    w->satisfies(argv[2][0]);
  } else {
    printf("warning: don't understand %s dependencies\n", argv[0]);
  }
  d->push(w);
}

void
cmd::print()
{
  for(int i = 0; i < argc; i++)
    printf("%s ", argv[i]);
  printf("\n");
}

sequence *
sequence::parse()
{
  sequence *s = new sequence();

  while(cmd *c = cmd::parse()){
    assert(s->n < 20);
    s->a[s->n++] = c;
  }

  return s;
}

void
sequence::run(dwq *d)
{
  for(int i = 0; i < n; i++){
    a[i]->run(d);
  }
  done = 1;
}

void
sequence::print()
{
  for(int i = 0; i < n; i++)
    a[i]->print();
}

int
main(void)
{
  initwq();
  dwq *d= new dwq();
  sequence *s = sequence::parse();
  if(s){
    s->run(d);
    d->run();
  }
}
