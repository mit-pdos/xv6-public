#include "types.h"
#include "defs.h"
#include "spinlock.h"

// name spaces
// XXX maybe use open hash table, no chain, better cache locality

#define NHASH 100

// XXX cache align
struct elem {
  int key;
  void *val;
  struct elem * volatile next;
};

struct bucket {
  struct spinlock l;
  struct elem * volatile chain;
};

struct ns {
  int used;
  int nextkey;
  struct bucket table[NHASH];
};

void
nsinit(void)
{
}

// XXX should be using our missing scalable allocator module
struct ns*
nsalloc(void)
{
  struct ns *ns = 0;

  ns = kmalloc(sizeof(struct ns));
  if (ns == 0)
    panic("nsalloc");
  memset(ns, 0, sizeof(struct ns));
  for (int i = 0; i < NHASH; i++)
    initlock(&ns->table[i].l, "bucket");
  ns->nextkey = 1;
  return ns;
}

static struct elem *
elemalloc(void)
{
  struct elem *e = 0;
  e = kmalloc(sizeof(struct elem));
  if (e == 0)
    return 0;
  memset(e, 0, sizeof(struct elem));
  return e;
}

// XXX need something more scalable; partition the name space?
int
ns_allockey(struct ns *ns)
{
  int n = __sync_fetch_and_add(&ns->nextkey, 1);
  return n;
}

int 
ns_insert(struct ns *ns, int key, void *val) 
{
  int r = -1;
  struct elem *e = elemalloc();
  if (e) {
    e->key = key;
    e->val = val;
    uint i = key % NHASH;
    rcu_begin_write(0);
    for (;;) {
      struct elem *x = ns->table[i].chain;
      e->next = x;
      if (__sync_bool_compare_and_swap(&ns->table[i].chain, x, e))
	break;
    }
    rcu_end_write(0);
    r = 0;
  }
  return r;
}

void*
ns_lookup(struct ns *ns, int key)
{
  uint i = key % NHASH;

  rcu_begin_read();
  struct elem *e = ns->table[i].chain;

  while (e != NULL) {
    if (e->key == key) {
      return e;
    }
    e = e->next;
  }
  rcu_end_read();

  return 0;
}

int
ns_remove(struct ns *ns, int key)
{
  uint i = key % NHASH;
  rcu_begin_write(&ns->table[i].l);

  struct elem * volatile * pe = &ns->table[i].chain;
  for (;;) {
    struct elem *e = *pe;
    if (!e)
      break;

    if (e->key == key) {
      for (;;)
	if (__sync_bool_compare_and_swap(pe, e, e->next))
	  break;
      rcu_end_write(&ns->table[i].l);
      rcu_delayed(e, kmfree);
      return 0;
    }

    pe = &e->next;
  }

  rcu_end_write(&ns->table[i].l);
  return -1;
}

void
ns_enumerate(struct ns *ns, void (*f)(int, void *))
{
  rcu_begin_read();
  for (int i = 0; i < NHASH; i++) {
    struct elem *e = ns->table[i].chain;
    while (e != NULL) {
      (*f)(e->key, e->val);
      e = e->next;
    }
  }
  rcu_end_read();
}

