#include "types.h"
#include "defs.h"
#include "spinlock.h"
#include "param.h"
#include <stddef.h>

// name spaces
// XXX maybe use open hash table, no chain, better cache locality

#if SPINLOCK_DEBUG
#define NHASH 10
#else
#define NHASH 100
#endif

struct elem {
  void *val;
  int next_lock;
  struct elem * volatile next;
  union {
    uint ikey;
    char skey[0];
  };
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
elemalloc(struct nskey *k)
{
  struct elem *e = 0;
  int sz;
  switch (k->type) {
  case nskey_int:
    sz = sizeof(*e);
    break;
  case nskey_str:
    sz = offsetof(struct elem, skey) + strlen(k->u.s) + 1;
    break;
  default:
    panic("key type");
  }
  
  e = kmalloc(sz);
  if (e == 0)
    return 0;
  memset(e, 0, sz);
  return e;
}

static int
h(struct nskey *k)
{
  switch (k->type) {
  case nskey_int:
    return k->u.i % NHASH;
  case nskey_str:
    return k->u.s[0] % NHASH; // XXX
  default:
    panic("key type");
  }
}

static void
setkey(struct elem *e, struct nskey *k)
{
  switch (k->type) {
  case nskey_int:
    e->ikey = k->u.i;
    break;
  case nskey_str:
    strncpy(e->skey, k->u.s, __INT_MAX__);
    break;
  default:
    panic("key type");
  }
}

static int
cmpkey(struct elem *e, struct nskey *k)
{
  switch (k->type) {
  case nskey_int:
    return e->ikey == k->u.i;
    break;
  case nskey_str:
    return !strcmp(e->skey, k->u.s);
    break;
  default:
    panic("key type");
  }
}

// XXX need something more scalable; partition the name space?
int
ns_allockey(struct ns *ns)
{
  uint n = __sync_fetch_and_add(&ns->nextkey, 1);
  return n;
}

int 
ns_insert(struct ns *ns, struct nskey key, void *val) 
{
  struct elem *e = elemalloc(&key);
  if (e) {
    setkey(e, &key);
    e->val = val;
    uint i = h(&key);
    rcu_begin_write(0);

   retry:
    (void) 0;
    struct elem *root = ns->table[i].chain;
    if (!ns->allowdup) {
      for (struct elem *x = root; x; x = x->next) {
	if (cmpkey(x, &key)) {
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
ns_lookup(struct ns *ns, struct nskey key)
{
  uint i = h(&key);

  rcu_begin_read();
  struct elem *e = ns->table[i].chain;

  while (e != NULL) {
    if (cmpkey(e, &key)) {
      rcu_end_read();
      return e->val;
    }
    e = e->next;
  }
  rcu_end_read();

  return 0;
}

int
ns_remove(struct ns *ns, struct nskey key, void *v)
{
  uint i = h(&key);
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

    if (cmpkey(e, &key) && (e->val == v || v == 0)) {
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
ns_enumerate(struct ns *ns, void *(*f)(void *, void *))
{
  rcu_begin_read();
  for (int i = 0; i < NHASH; i++) {
    struct elem *e = ns->table[i].chain;
    while (e != NULL) {
      void *r = (*f)(&e->ikey, e->val);
      if (r) {
	rcu_end_read();
	return r;
      }
      e = e->next;
    }
  }
  rcu_end_read();
  return 0;
}

void *
ns_enumerate_key(struct ns *ns, struct nskey key, void *(*f)(void *))
{
  uint i = h(&key);
  rcu_begin_read();
  struct elem *e = ns->table[i].chain;
  while (e) {
    if (cmpkey(e, &key)) {
      void *r = (*f)(e->val);
      if (r) {
	rcu_end_read();
	return r;
      }
    }
    e = e->next;
  }
  rcu_end_read();
  return 0;
}

