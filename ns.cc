extern "C" {
#include "types.h"
#include "kernel.h"
#include "spinlock.h"
#include "fs.h"
#include <stddef.h>
}

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
    u64 ikey;
    struct {
      u64 a;
      u64 b;
    } iikey;
    char skey[0];
    char dnkey[DIRSIZ];
    struct {
      u64 a;
      u64 b;
      char s[0];
    } iiskey;
  };
};

struct bucket {
  struct elem * volatile chain;
} __attribute__((aligned (CACHELINE)));

struct ns {
  int allowdup;
  u64 nextkey;
  struct bucket table[NHASH];
};


// XXX should be using our missing scalable allocator module
struct ns*
nsalloc(int allowdup)
{
  struct ns *ns = 0;

  ns = (struct ns*) kmalloc(sizeof(struct ns));
  if (ns == 0)
    panic("nsalloc");
  memset(ns, 0, sizeof(struct ns));
  ns->nextkey = 1;
  ns->allowdup = allowdup;
  return ns;
}

static void *
any(void *x, void *y, void *arg)
{
  return (void*) 1;
}

void
nsfree(struct ns *ns)
{
  if (ns_enumerate(ns, &any, 0))
    panic("nsfree: not empty");
  gc_delayed(ns, kmfree);
}

static struct elem *
elemalloc(struct nskey *k)
{
  struct elem *e = 0;
  int sz;
  switch (k->type) {
  case nskey_int:
    sz = offsetof(struct elem, ikey) + sizeof(e->ikey);
    break;
  case nskey_ii:
    sz = offsetof(struct elem, iikey) + sizeof(e->iikey);
    break;
  case nskey_str:
    sz = offsetof(struct elem, skey) + strlen(k->u.s) + 1;
    break;
  case nskey_dirname:
    sz = offsetof(struct elem, dnkey) + sizeof(e->dnkey);
    break;
  case nskey_iis:
    sz = offsetof(struct elem, iiskey.s) + strlen(k->u.iis.s) + 1;
    break;
  default:
    panic("key type");
  }

  e = (elem*) kmalloc(sz);
  if (e == 0)
    return 0;
  memset(e, 0, sz);
  return e;
}

static u64
h(struct nskey *k)
{
  switch (k->type) {
  case nskey_int:
    return k->u.i % NHASH;
  case nskey_ii:
    return (k->u.ii.a ^ k->u.ii.b) % NHASH;
  case nskey_str:
    return k->u.s[0] % NHASH; // XXX
  case nskey_dirname:
    return k->u.dirname[0] % NHASH; // XXX
  case nskey_iis:
    return (k->u.iis.a ^ k->u.iis.b ^ k->u.iis.s[0]) % NHASH;
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
  case nskey_ii:
    e->iikey.a = k->u.ii.a;
    e->iikey.b = k->u.ii.b;
    break;
  case nskey_str:
    strncpy(e->skey, k->u.s, strlen(k->u.s) + 1);
    break;
  case nskey_dirname:
    strncpy(e->dnkey, k->u.dirname, DIRSIZ);
    break;
  case nskey_iis:
    e->iiskey.a = k->u.iis.a;
    e->iiskey.b = k->u.iis.b;
    strncpy(e->iiskey.s, k->u.iis.s, strlen(k->u.iis.s) + 1);
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
  case nskey_ii:
    return e->iikey.a == k->u.ii.a && e->iikey.b == k->u.ii.b;
  case nskey_str:
    return !strcmp(e->skey, k->u.s);
  case nskey_dirname:
    return !namecmp(e->dnkey, k->u.dirname);
  case nskey_iis:
    return e->iiskey.a == k->u.iis.a &&
	   e->iiskey.b == k->u.iis.b &&
	   !strcmp(e->iiskey.s, k->u.iis.s);
  default:
    panic("key type");
  }
}

// XXX need something more scalable; partition the name space?
u64
ns_allockey(struct ns *ns)
{
  u64 n = __sync_fetch_and_add(&ns->nextkey, 1);
  return n;
}

int 
ns_insert(struct ns *ns, struct nskey key, void *val) 
{
  struct elem *e = elemalloc(&key);
  if (e) {
    setkey(e, &key);
    e->val = val;
    u64 i = h(&key);
    gc_begin_epoch();

   retry:
    (void) 0;
    struct elem *root = ns->table[i].chain;
    if (!ns->allowdup) {
      for (struct elem *x = root; x; x = x->next) {
	if (cmpkey(x, &key)) {
	  gc_end_epoch();
	  gc_delayed(e, kmfree);
	  return -1;
	}
      }
    }

    e->next = root;
    if (!__sync_bool_compare_and_swap(&ns->table[i].chain, root, e))
      goto retry;

    gc_end_epoch();
    return 0;
  }
  return -1;
}

void*
ns_lookup(struct ns *ns, struct nskey key)
{
  u64 i = h(&key);

  gc_begin_epoch();
  struct elem *e = ns->table[i].chain;

  while (e != NULL) {
    if (cmpkey(e, &key)) {
      gc_end_epoch();
      return e->val;
    }
    e = e->next;
  }
  gc_end_epoch();

  return 0;
}

void*
ns_remove(struct ns *ns, struct nskey key, void *v)
{
  u64 i = h(&key);
  gc_begin_epoch();

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
      void *v = e->val;
      gc_end_epoch();
      gc_delayed(e, kmfree);
      return v;
    }

    pe = &e->next;
    pelock = &e->next_lock;
  }

  gc_end_epoch();
  return 0;
}

void *
ns_enumerate(struct ns *ns, void *(*f)(void *, void *, void *), void *arg)
{
  gc_begin_epoch();
  for (int i = 0; i < NHASH; i++) {
    struct elem *e = ns->table[i].chain;
    while (e != NULL) {
      void *r = (*f)(&e->ikey, e->val, arg);
      if (r) {
	gc_end_epoch();
	return r;
      }
      e = e->next;
    }
  }
  gc_end_epoch();
  return 0;
}

void *
ns_enumerate_key(struct ns *ns, struct nskey key, void *(*f)(void *, void *), void *arg)
{
  u64 i = h(&key);
  gc_begin_epoch();
  struct elem *e = ns->table[i].chain;
  while (e) {
    if (cmpkey(e, &key)) {
      void *r = (*f)(e->val, arg);
      if (r) {
	gc_end_epoch();
	return r;
      }
    }
    e = e->next;
  }
  gc_end_epoch();
  return 0;
}
