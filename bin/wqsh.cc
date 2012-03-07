#include "types.h"
#include "user.h"
#include "fcntl.h"
#include "wq.hh"

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
  virtual void run() = 0;
  int checkdone() { return done; }
};

class cmd : public thing {
private:
  int argc;
  char *argv[20];
public:
  cmd() { argc = 0; }

  void print();
  void run();
  static cmd *parse();
};

class sequence : public thing {
private:
  int n;
  class cmd *a[20];
public:
  sequence() { n = 0; }

  void print();
  void run();
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
cmd::run()
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
blah(void *x)
{
  thing *t = (thing *) x;
  t->run();
}

void
sequence::run()
{
  for(int i = 0; i < n; i++){
    cwork *w = new cwork();
    w->rip = (void*) blah;
    w->arg0 = a[i];
    wq_push(w);
  }
  while(1){
    int do_more = 0;
    for(int i = 0; i < n; i++){
      if(a[i]->checkdone() == 0){
        do_more += 1;
      }
    }
    if(do_more == 0)
      break;
    wq_trywork();
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
  sequence *s = sequence::parse();
  if(s){
    s->run();
  }
}
