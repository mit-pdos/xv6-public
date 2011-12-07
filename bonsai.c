#include "types.h"
#include "kernel.h"
#include "mmu.h"
#include "spinlock.h"

/*
 * Balanced tree: Implementation based on
 * http://groups.csail.mit.edu/mac/users/adams/BB/, which makes supporting
 * concurrent reads with locked-writes easy, because updates are
 * non-destructive.  As an optimization, we turn the easy cases into destructive
 * updates.
 *
 * XXX the rotates could all lose one rcu_free by updating the pointers involved
 * in the right order (i.e., the parent second); unfortunately, it makes the
 * less "functional".
 */


#define WEIGHT 4
#define SIZE(n) ((n == 0) ? 0 : n->size)

typedef struct node node_t;
struct node {
  struct kv kv;
  int size;
  node_t *r, *l;
} __mpalign__;

static node_t*
alloc_node(struct kv *kv, node_t *l, node_t *r)
{
  node_t *n = kmalloc(sizeof(node_t));
  if (n == 0)
    return 0;
  n->kv = *kv;
  n->size = SIZE(l) + SIZE(r) + 1;
  n->l = l;
  n->r = r;
  return n;
}

static void
free_node(void *p)
{
  kmfree(p);
}

static void
rcu_free_node(node_t *n)
{
  gc_delayed(n, free_node);
}

static void 
tree_printspace(int s)
{
  int i;
  
  for (i = 0; i < s; i++)
    cprintf(" ");
}

void
tree_print(node_t *n, int depth)
{
  if (n == 0) return;

  if (n->l != 0)
    tree_print(n->l, depth+1);
  tree_printspace(depth*2);
  cprintf("0x%lx: %d sz %d l 0x%lx 0x%lx\n", (long) n, n->kv.key, n->size, 
	 (long) n->l, (long) n->r);
  if (n->r != 0)
    tree_print(n->r, depth+1);
}

struct kv*
tree_find(node_t *n, u64 key)
{
  if (n == 0) return 0;

  if (n->kv.key == key) return &n->kv;

  if (key < n->kv.key) return tree_find(n->l, key);
  else return tree_find(n->r, key);
}

struct kv*
tree_find_gt(node_t *n, u64 key)
{
  struct kv *r = 0;

  if (n == 0) return 0;

  if (n->kv.key > key) {
    r = tree_find_gt(n->l, key);
    if (r == 0) r = &n->kv;
  } else {
    r = tree_find_gt(n->r, key);
  }
  return r;
}

static node_t* tree_balance(node_t *n, node_t *l, node_t *r, int, int);

static node_t*
tree_single_left(struct kv *kv, node_t *l, node_t *r)
{
  node_t *t = alloc_node(&r->kv, alloc_node(kv, l, r->l), r->r);
  rcu_free_node(r);
  return t;
}

static node_t*
tree_double_left(node_t *n, node_t *l, node_t *r)
{
  node_t *tl = alloc_node(&n->kv, l,  r->l->l);
  // node_t *tr = alloc_node(r->key, r->l->r, r->r);
  node_t *tr = tree_balance(r, r->l->r, r->r, 0, 0);
  node_t *t = alloc_node(&r->l->kv, tl, tr);

  rcu_free_node(r->l);
  // rcu_free_node(r);
  return t;
}

static node_t*
tree_single_right(struct kv *kv, node_t *l, node_t *r)
{
  node_t *t = alloc_node(&l->kv, l->l, alloc_node(kv, l->r, r));
  rcu_free_node(l);
  return t;
}

static node_t*
tree_double_right(node_t *n, node_t *l, node_t *r)
{
  // node_t *tl = alloc_node(l->key, l->l, l->r->l);
  node_t *tl = tree_balance(l, l->l, l->r->l, 0, 0);
  node_t *tr = alloc_node(&n->kv, l->r->r, r);
  node_t *t = alloc_node(&l->r->kv, tl, tr);

  rcu_free_node(l->r);
  // rcu_free_node(l);
  return t;
}

static node_t*
tree_node(node_t *n, node_t *l, node_t *r, int leftreplace, int inplace)
{
  node_t *t;

  if (inplace) {
    if (leftreplace) {
      n->l = l;
    } else {
      n->r = r;
    }
    n->size = 1 + SIZE(l) + SIZE(r);
    t = n;
  } else {
    t = alloc_node(&n->kv, l, r);
    rcu_free_node(n);
  }
  return t;
}

/* Returns a balanced tree for n after r or l have grown by one. */
static node_t*
tree_balance(node_t *n, node_t *l, node_t *r, int leftreplace, int inplace)
{
  int ln = SIZE(l);
  int rn = SIZE(r);
  node_t *t;

  //  printf("tree_balance: (%d,%d,0x%lx, 0x%lx) leftr %d inplace %d\n", n->key, 
  //	 n->val, (long) l, (long) r, leftreplace, inplace);
  if (ln + rn < 2) {
    t = tree_node(n, l, r, leftreplace, inplace);
  } else if (rn > WEIGHT * ln) {
    int rln = SIZE(r->l);
    int rrn = SIZE(r->r);
    if (rln < rrn) t = tree_single_left(&n->kv, l, r);
    else t = tree_double_left(n, l, r);
    rcu_free_node(n);
  } else if (ln > WEIGHT * rn) {
    int lln = SIZE(l->l);
    int lrn = SIZE(l->r);
    if (lrn < lln) t = tree_single_right(&n->kv, l, r);
    else t = tree_double_right(n, l, r);
    rcu_free_node(n);
  } else {
    t = tree_node(n, l, r, leftreplace, inplace);
  }
  return t;
}

node_t*
tree_insert(node_t *n, struct kv *kv)
{
  node_t* t;
  if (n == 0) {
    t = alloc_node(kv, 0, 0);
  } else if (kv->key == n->kv.key) {
    t = n;
  } else if (kv->key < n->kv.key) {
    t = tree_balance(n, tree_insert(n->l, kv), n->r, 1, 1);
  } else {
    t = tree_balance(n, n->l, tree_insert(n->r, kv), 0, 1);
  }
  return t;
}

static node_t* 
tree_min(node_t *n)
{
  if (n->l == 0) return n;
  else return tree_min(n->l);
}

static node_t*
tree_delmin(node_t *n)
{
  node_t *t;

  if (n->l == 0) {
    t = n->r;
  } else {
    /* rebuild branch with min non-destructively, because readers
     * might be going and must see min */
    t = tree_balance(n, tree_delmin(n->l), n->r, 1, 0);
  }
  return t;
}

static node_t*
tree_delprime(node_t *l, node_t *r)
{
  node_t *t;
  if (l == 0) t = r;
  else if (r == 0) t = l;
  else {
    node_t *min = tree_min(r);   // XXX combine with tree_delmin?
    /* min is still linked below in the tree, so build new min
     * non-destructively. the new min will be inserted atomically by
     * parent. */
    t = tree_balance(min, l, tree_delmin(r), 0, 0);
  }
  return t;
}

node_t*
tree_remove(node_t *n, u64 key)
{
  node_t *t;

  if (n == 0) t = 0;
  else if (key < n->kv.key) {
    t = tree_balance(n, tree_remove(n->l, key), n->r, 1, 1);
  } else if (key > n->kv.key) {
    t = tree_balance(n, n->l, tree_remove(n->r, key), 0, 1);
  } else { 
    t = tree_delprime(n->l, n->r);
    rcu_free_node(n);
  }
  return t;
}

int
tree_foreach(node_t *n, int (*cb)(struct kv*, void *), void *st)
{
  if (n == 0)
    return 1;

  if (!tree_foreach(n->l, cb, st))
    return 0;
  if (!cb(&n->kv, st))
    return 0;
  
  if (!tree_foreach(n->r, cb, st))
    return 0;
  return 1;
}

#define TEST
#ifdef TEST

#define NINSERT 1000

static struct node *root;
static int insert_values[NINSERT+1];

int
tree_check(node_t *n)
{
  int t = 0;

  if (n == 0) return 0;

  if (n->l != 0) t = tree_check(n->l);

  // in order?
  if (n->l != 0 && n->l->kv.key > n->kv.key) {
    panic("out of order l");
  }
  if (n->r != 0 && n->r->kv.key < n->kv.key) {
    panic("out of order r");
  }

  // in balance?
  int ln = SIZE(n->l);
  int rn = SIZE(n->r);
  if (((ln > 0) && (rn >0)) && ((rn > ln * WEIGHT) || (ln > rn * WEIGHT))) {
    panic("imbalanace");
  }

  if (n->r != 0) t += tree_check(n->r);

  // size correct?
  if (n->size != t + 1) {
    cprintf("size %d %d\n", n->size, t + 1);
    panic("incorrect size");
  }
  return t + 1;
}

void
rcu_tree_check(int t, int sz)
{
  int i;
  struct kv *kv;

  // are all keys in the tree?
  for (i = 0; i < sz; i++) {
    if (insert_values[i] == 0)
      continue;
    kv = tree_find(root, insert_values[i]);
    if (kv == 0) {
      cprintf("tree_check %d (%d) fails\n", insert_values[i], i);
      panic("key missing");
    }
  }
  tree_check(root);
}


static int
draw(int c)
{
  static int seed = 37;
  seed = seed * 1103515245 + 12345; 
  return seed & 0xFFFFFFF;
}

void
tree_test(void)
{
  int i;
  for (i = 0; i < NINSERT; i++) {
    struct kv kv;
    long d = draw(0);
    kv.key = d;
    insert_values[i] = d;
    // tree_print(root, 0);
    // printf("add(%ld,%ld) nfree %d\n", d, d, tree_nfree);
    root = tree_insert(root, &kv);
  }
  rcu_tree_check(0, NINSERT);
  cprintf("tree_test ok1\n");
  for (i = 0; i < NINSERT; i = i + 2) {
    root = tree_remove(root, insert_values[i]);
    insert_values[i] = 0;
  }
  rcu_tree_check(0, NINSERT);
  cprintf("tree_test ok2\n");
}
#endif
