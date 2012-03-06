#include "types.h"
#include "user.h"
#include "fcntl.h"

class thing {
public:
  static void * operator new(unsigned long n){
    return malloc(n);
  }
  static void operator delete(void *p){
    free(p);
  }
  virtual void print();
};

class cmd : public thing {
public:
  cmd();
  virtual ~cmd();
  int argc;
  char *argv[20];

  void print();
  static cmd *parse();
};

class sequence : public thing {
public:
  sequence();
  virtual ~sequence();
  int n;
  class cmd *a[20];

  void print();
  static sequence *parse();
};

cmd::cmd()
{
  argc = 0;
}

cmd::~cmd()
{
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
    for(j = i; buf[j] && buf[j] != ' '; j++)
      ;

    char *p = (char *) malloc(j - i + 1);
    assert(p);
    memmove(p, buf + i, j - i);
    p[j - i] = '\0';
    assert(c->argc < 20);
    c->argv[c->argc++] = p;

    while(buf[j] == ' ')
      j++;
    i = j;
  }
  return c;
}

void
cmd::print()
{
  for(int i = 0; i < argc; i++)
    printf("%s ", argv[i]);
  printf("\n");
}

sequence::sequence()
{
  n = 0;
}

sequence::~sequence()
{
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
sequence::print()
{
  for(int i = 0; i < n; i++)
    a[i]->print();
}

int
main(void)
{
  sequence *s = sequence::parse();
  if(s){
    s->print();
  }
}
