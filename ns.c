#include "types.h"
#include "defs.h"
#include "spinlock.h"
#include "queue.h"

// name spaces
// XXX maybe use open hash table, no chain, better cache locality

#define NHASH 100
static int rcu = 0;

#define ACQUIRE(l) (rcu) ? rcu_begin_write(l) : acquire(l)
#define RELEASE(l) (rcu) ? rcu_end_write(l) : release(l)

// XXX cache align
struct elem {
  int key;
  void *val;
  TAILQ_ENTRY(elem) chain;
};

struct bucket {
  TAILQ_HEAD(elist, elem) chain;
};

struct ns {
  int used;
  int nextkey; 
  struct bucket table[NHASH];
  struct spinlock lock;
};

struct spinlock ns_lock;

void
nsinit(void)
{
  initlock(&ns_lock, "nstable");
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

  acquire(&ns_lock);
  for (int i = 0; i < NHASH; i++) {
    TAILQ_INIT(&ns->table[i].chain);
  }
  release(&ns_lock);
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
  ACQUIRE(&ns->lock);
  struct elem *e = elemalloc();
  if (e) {
    e->key = key;
    e->val = val;
    uint i = key % NHASH;
    TAILQ_INSERT_TAIL(&(ns->table[i].chain), e, chain);
    r = 0;
  }
  RELEASE(&ns->lock);
  return r;
}

static struct elem*
ns_dolookup(struct ns *ns, int key)
{
  struct elem *e = TAILQ_FIRST(&(ns->table[key % NHASH].chain));
  while (e != NULL) {
    if (e->key == key) {
      return e;
    }
    e = TAILQ_NEXT(e, chain);
  }
  return 0;
}

void*
ns_lookup(struct ns *ns, int key)
{
  ACQUIRE(&ns->lock);
  struct elem *e = ns_dolookup(ns, key);
  RELEASE(&ns->lock);
  return (e == 0)? 0 : e->val;
}

int
ns_remove(struct ns *ns, int key)
{
  int r = -1;
  ACQUIRE(&ns->lock);
  struct elem *e = ns_dolookup(ns, key);
  if (e) {
    TAILQ_REMOVE(&(ns->table[key % NHASH].chain), e, chain);
    kmfree(e);
    r = 0;
  }
  RELEASE(&ns->lock);
  return r;
}

void
ns_enumerate(struct ns *ns, void (*f)(int, void *))
{
  acquire(&ns->lock);
  for (int i = 0; i < NHASH; i++) {
    struct elem *e = TAILQ_FIRST(&(ns->table[i].chain));
    while (e != NULL) {
      (*f)(e->key, e->val);
      e = TAILQ_NEXT(e, chain);
    }
  }
  release(&ns->lock);
}
