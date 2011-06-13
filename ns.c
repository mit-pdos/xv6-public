#include "types.h"
#include "defs.h"
#include "spinlock.h"
#include "param.h"

// name spaces
// XXX maybe use open hash table, no chain, better cache locality

#if SPINLOCK_DEBUG
#define NHASH 10
#else
#define NHASH 100
#endif

struct elem {
  uint key;
  void *val;
  int next_lock;
  struct elem * volatile next;
};

struct bucket {
  struct elem * volatile chain;
} __attribute__((aligned (CACHELINE)));

struct ns {
  int allowdup;
  uint nextkey;
  struct bucket table[NHASH];
};

void
nsinit(void)
{
}

// XXX should be using our missing scalable allocator module
struct ns*
nsalloc(int allowdup)
{
  struct ns *ns = 0;

  ns = kmalloc(sizeof(struct ns));
  if (ns == 0)
    panic("nsalloc");
  memset(ns, 0, sizeof(struct ns));
  ns->nextkey = 1;
  ns->allowdup = allowdup;
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
  uint n = __sync_fetch_and_add(&ns->nextkey, 1);
  return n;
}

int 
ns_insert(struct ns *ns, uint key, void *val) 
{
  struct elem *e = elemalloc();
  if (e) {
    e->key = key;
    e->val = val;
    uint i = key % NHASH;
    rcu_begin_write(0);

   retry:
    (void) 0;
    struct elem *root = ns->table[i].chain;
    if (!ns->allowdup) {
      for (struct elem *x = root; x; x = x->next) {
	if (x->key == key) {
	  rcu_end_write(0);
	  rcu_delayed(e, kmfree);
	  return -1;
	}
      }
    }

    e->next = root;
    if (!__sync_bool_compare_and_swap(&ns->table[i].chain, root, e))
      goto retry;

    rcu_end_write(0);
    return 0;
  }
  return -1;
}

void*
ns_lookup(struct ns *ns, uint key)
{
  uint i = key % NHASH;

  rcu_begin_read();
  struct elem *e = ns->table[i].chain;

  while (e != NULL) {
    if (e->key == key) {
      return e->val;
    }
    e = e->next;
  }
  rcu_end_read();

  return 0;
}

int
ns_remove(struct ns *ns, uint key, void *v)
{
  uint i = key % NHASH;
  rcu_begin_write(0);

 retry:
  (void) 0;

  int fakelock = 0;
  int *pelock = &fakelock;
  struct elem * volatile * pe = &ns->table[i].chain;
  for (;;) {
    struct elem *e = *pe;
    if (!e)
      break;

    if (e->key == key && (e->val == v || v == 0)) {
      // XXX annotate as locks for mtrace
      if (!__sync_bool_compare_and_swap(&e->next_lock, 0, 1))
	goto retry;
      if (!__sync_bool_compare_and_swap(pelock, 0, 1)) {
	e->next_lock = 0;
	goto retry;
      }
      if (!__sync_bool_compare_and_swap(pe, e, e->next)) {
	*pelock = 0;
	e->next_lock = 0;
	goto retry;
      }

      *pelock = 0;
      rcu_end_write(0);
      rcu_delayed(e, kmfree);
      return 0;
    }

    pe = &e->next;
  }

  rcu_end_write(0);
  return -1;
}

void *
ns_enumerate(struct ns *ns, void *(*f)(uint, void *))
{
  rcu_begin_read();
  for (int i = 0; i < NHASH; i++) {
    struct elem *e = ns->table[i].chain;
    while (e != NULL) {
      void *r = (*f)(e->key, e->val);
      if (r)
	return r;
      e = e->next;
    }
  }
  rcu_end_read();
  return 0;
}

void *
ns_enumerate_key(struct ns *ns, uint key, void *(*f)(void *))
{
  uint i = key % NHASH;
  rcu_begin_read();
  struct elem *e = ns->table[i].chain;
  while (e) {
    if (e->key == key) {
      void *r = (*f)(e->val);
      if (r)
	return r;
    }
    e = e->next;
  }
  rcu_end_read();
  return 0;
}

