#include "types.h"
#include "kernel.h"
#include "mmu.h"
#include "spinlock.h"
#include "cpu.h"

//
// Concurrent atomic ranges operations using skip lists.  Overall approach is
// that the bottom list contains the truth, and the index is an
// performance-acceleration hint.  The implementation builds up the index
// lazily.
//
// Searches run without locks.  Changes to the list take locks on the nodes that
// are involved in the change.  Thus, if two changes are to different parts of
// the list, they can happen in parallel. The invariant for deletion and
// insertion is that we always lock the predecessor node. This
// ensures we avoid the races for the skip list implementation (see skip_add()).
// The locks ensure that range operations are atomic.
//
// A tricky case is that when we found the nodes corresponding to a range, we
// must lock the predecessor first and then the first node of the range.  But,
// while waiting for the lock for the predecessor, another core may be deleting
// the predecessor. To handle this race, the core that deletes a node marks a
// node for deletion by marking its next pointer.
//

#define CRANGE_CHECKING 0
#define MINNLEVEL 10

#define MARKED(x) (((uptr) (x)) & 0x1)
#define RANGE_MARK(x) ((struct clist_range *) (((uptr) (x)) | 0x1))
#define RANGE_WOMARK(x) ((struct clist_range *) (((uptr) (x)) & ~0x1))

enum { crange_debug = 0 };

struct crange {
  int nlevel;
  struct clist_range crange_head;
} cr;

struct crange*
crange_alloc(int nlevel)
{
  struct crange *cr;
  assert(kmalign((void **) &cr, CACHELINE, sizeof(struct crange)) == 0);
  cr->nlevel = (nlevel < MINNLEVEL) ? MINNLEVEL : nlevel;  // XXX
  cr->crange_head.cr = cr;
  assert(kmalign((void **) &cr->crange_head.lock, 
			   CACHELINE, sizeof(struct spinlock)) == 0);
  initlock(cr->crange_head.lock, "head lock");
  cr->crange_head.next = kmalloc(sizeof(cr->crange_head.next[0]) * nlevel);
  for (int l = 0; l < nlevel; l++) cr->crange_head.next[l] = 0;
  if (crange_debug) cprintf("crange_alloc: return 0x%lx\n", (u64) cr);
  return cr;
}

static void clist_range_free(void *p);

void
crange_free(struct crange *cr)

{
  assert(cr);
  if (crange_debug) cprintf("crange_free: 0x%lx\n", (u64) cr);
  struct clist_range *e, *n;
  for (e = RANGE_WOMARK(cr->crange_head.next[0]); e; e = n) {
    n = RANGE_WOMARK(e->next[0]);
    clist_range_free(e);
  }
  kmfree(cr->crange_head.next);
  kmalignfree(cr->crange_head.lock);
  kmalignfree(cr);
}

// draw nlevel in [1, nlevel]
static int
crange_draw_nlevel(int nlevel)
{
  int l;
  for (l = 0; l < nlevel-1; l++) {
    if (rnd() % 16777216 > 8388608)    // % 1 is not enough randomness
      //if (random() % 2 == 1) 
      break;
  }
  return l+1;
}

void 
crange_print_elem(struct clist_range *e, int l)
{
  cprintf ("0x%lx(0x%lx, 0x%lx, c %d, t %d, l %d, n 0x%lx m 0x%lx\n", e->key, e->size, (long) e->value, e->curlevel, e->nlevel, e->lock->locked, (long) e->next, MARKED(e->next[l]));
}

void
crange_print(struct crange *cr, int full)
{
  struct clist_range *e;
  for (int l = 0; l < cr->nlevel; l++) {
    int c = 0;
    cprintf("crange %d (l %d): ", l, cr->crange_head.lock->locked);
    for (e = cr->crange_head.next[l]; e; e = RANGE_WOMARK(e->next[l])) {
      c++;
      if (full) crange_print_elem(e, l);
    }
    cprintf(" cnt %d \n", c);
  }
}

void
crange_check(struct crange *cr, int lockcheck, struct clist_range *absent)
{
  if (!CRANGE_CHECKING)
    return;
  int t = mycpu()->id;
  struct clist_range *e, *s, *p;
  for (int l = 0; l < cr->nlevel; l++) {
    p = &cr->crange_head;
    for (e = cr->crange_head.next[l]; e; e = s) {
      assert((e != absent) || (l == 0 && e->lock->locked));
      // assert(!lockcheck || e->lock.me != mycpu()->id);
      assert(e->curlevel < cr->nlevel);
      if (l > 0 && e->next[l] != 0 && !MARKED(e->next[l])) {
	struct clist_range *n;
	// look for e level down
	for (n = RANGE_WOMARK(p->next[l-1]); n && n != e; n = RANGE_WOMARK(n->next[l-1]))
	  ;
	if (!MARKED(e->next[l]) && n != e) {
	  cprintf("%d: check level %d failed %lu(%lu); in high level but not low?\n", t, l, e->key,
		 e->size);
	  crange_print(cr, 1);
	  assert(0);
	}
      }
      if (l < e->curlevel && e->curlevel < e->nlevel-1) {
	struct clist_range *n;
	// look for e one level up
	for (n = RANGE_WOMARK(cr->crange_head.next[l+1]); n && n != e; n = RANGE_WOMARK(n->next[l+1]))
	  ;
	if (n != e && e->curlevel > l) {
	  // cprintf("%d: check level %d %u(%u) failed; not in level %d\n", t, l, e->key, e->size, e->curlevel);
	  // crange_print(1);
	  // assert(0);
	}
      }
      s = RANGE_WOMARK(e->next[l]);
      assert(s != e);
      if (!MARKED(e->next[l]) && s && (e->key + e->size > s->key)) {
	if (crange_debug) cprintf("%d: e(%lu,%lu) overlaps with s(%lu,%lu)\n", t, e->key, e->size, s->key, e->size);
	crange_print(cr, 1);
	assert(0);
      }
      if (!MARKED(e->next[l])) p = e;
    }
  }
}

static void
clist_range_free(void *p)
{
  struct clist_range *e = (struct clist_range *) p;
  crange_check(e->cr, 0, p);
  for (int l = 0; l < e->nlevel; l++) {
    e->next[l] = (struct clist_range *) 0xDEADBEEF;
  }
  kmalignfree(e->lock);
  kmfree(e->next);
  kmalignfree(e);
}

static void
crange_free_delayed(struct clist_range *e)
{
  // cprintf("crange_free_delayed: 0x%lx %u(%u) %u\n", (long) e, (e)->key, (e)->size, myepoch);
  crange_check(e->cr, 0, e);
  gc_delayed(e, clist_range_free);
}

static struct clist_range *
crange_new(struct crange *cr, u64 k, u64 sz, void *v, struct clist_range *n)
{
  struct clist_range *r;
  kmalign((void **) &r, CACHELINE, sizeof(struct clist_range));
  assert(r);
  r->key = k;
  r->size = sz;
  r->value = v;
  assert(r->size > 0);
  assert(cr->nlevel > 0);
  r->curlevel = 0;
  r->nlevel = crange_draw_nlevel(cr->nlevel);
  r->next = kmalloc(sizeof(sizeof(r->next[0])) * r->nlevel); // cache align?
  assert(r->next);
  r->next[0] = n;
  for (int l = 1; l < r->nlevel; l++) r->next[l] = 0;
  assert(kmalign((void **) &r->lock, CACHELINE, 
			   sizeof(struct spinlock)) == 0);
  initlock(r->lock, "crange");
  r->cr = cr;
  return r;
}

static struct clist_range *
crange_insert(struct clist_range *head, struct clist_range *new)
{
  struct clist_range *n, *p;
  p = NULL;
  for (n = head; n != 0; p = n, n = n->next[0]) {
    assert(!MARKED(n));
    if (new->key < n->key) {
      break;
    }
  }
  if (n == head) {
    new->next[0] = head;
    head = new;
  } else {
    p->next[0] = new;
    new->next[0] = n;
  }
  return head;
}

// does [k1, sz1) intersect with [k2, sz2)?
static int 
crange_intersect(u64 k1, u64 sz1, u64 k2, u64 sz2)
{
  if (k1+sz1 <= k2) return 0;  // region1 < region2
  else if (k1 >= k2+sz2) return 0;  // region1  > region2
  else return 1;
}

// lock p if p->next == e and p isn't marked for deletion.  if not, return failure.
static int
crange_lockpred(struct clist_range *p, struct clist_range *e)
{
  assert(!MARKED(e));
  acquire(p->lock);
  if (p->next[0] == e) {
      return 1;
  } 
  release(p->lock);
  // cprintf("%d: crange_lockpred: retry %u\n", mycpu()->id, p->key);
  return 0;
}

// Mark nodes f till s for deletion from top-level down through level l
static void 
crange_mark(struct clist_range *f, struct clist_range *s, int level) 
{
  struct clist_range *e;
  for (e = f; e && e != s; e = RANGE_WOMARK(e->next[0])) {
    assert(e);
    for (int l = e->nlevel-1; l >= level; l--) {
      assert(l < e->nlevel);
      (void) __sync_fetch_and_or(&(e->next[l]), 0x1);
    }
  }
}

// Unlock nodes f through l
static void 
crange_unlockn(struct clist_range *f, struct clist_range *l)
{
  struct clist_range *e;
  for (e = f; e != l; e = RANGE_WOMARK(e->next[0])) {
    assert(e);
    release(e->lock);
  }
  if (l) release(e->lock);
}

// Delay free nodes f through l
static void 
crange_freen(struct clist_range *f, struct clist_range *l)
{
  struct clist_range *e;
  for (e = f; e != l; e = RANGE_WOMARK(e->next[0])) {
    assert(e);
    if (e->curlevel == 0) {
      crange_free_delayed(e);
    }
  }
  if (l && l->curlevel == 0) {
    crange_free_delayed(e);
  }
}

// Remove e from index, if marked for deletion.  Returns 1 if e isn't marked.
// Returns 0, if marked but on level 0. Returns -1 if remove fails.
// Returns 1 on success.  Tricky because of races between add and del.
static int
crange_del_index(struct crange *cr, struct clist_range *p0, struct clist_range **e, int l)
{
  int r = 1;
  assert(l < (*e)->nlevel);
  if (!MARKED((*e)->next[l]))  // don't remove unmarked nodes from index
    return r;
  if (l == 0) return 0;    // but not on level 0; they are locked when removed
  // crange_check(0, NULL);
  while (*e && MARKED((*e)->next[l])) {
    if (l != (*e)->curlevel) {
      // node is still in the index one level up, back out.  we want to remove it first 
      // at higher levels so that we ensure the invariant nodes are removed top down.
      // cprintf("%d: crange_del_index: retry del %d %u(%u) on level %d\n", mycpu()->id, l, (*e)->key, (*e)->size,  (*e)->curlevel);
      r = -1;
      goto done;
    }
    int cas = __sync_bool_compare_and_swap(&(p0->next[l]), *e, RANGE_WOMARK((*e)->next[l]));
    if (cas) {
      __sync_fetch_and_sub(&((*e)->curlevel), 1);
      if (l == 1 && !(*e)->lock->locked) {
	// assert((*e)->curlevel == 0);   // XXX could it be 1 temporarily because of add_index?
	crange_free_delayed(*e);
      }
      *e = RANGE_WOMARK((*e)->next[l]);
    } else {  
      // cprintf("%d: crange_del_index: retry del %u(%u)\n", mycpu()->id, (*e)->key, (*e)->size);
      r = -1;
      goto done;
    }
  }
 done:
  crange_check(cr, 0, NULL);
  return r;
}

// Insert e into index one level up, between p and s, if e hasn't been inserted
// yet on that level.
static void
crange_add_index(struct crange *cr, int l, struct clist_range *e, struct clist_range *p1, struct clist_range *s1)
{
  if (l >= e->nlevel-1) return;
  if (MARKED(e->next[l+1])) return;
  // crange_check(0, NULL);
  if (__sync_bool_compare_and_swap(&e->curlevel, l, l+1)) {
    assert(e->curlevel < e->nlevel);
    // this is the core inserting at level l+1, but some core may be deleting
    struct clist_range *s = RANGE_WOMARK(s1);
    do {
      struct clist_range *n = e->next[l+1];
      if (MARKED(n)) {
	// this node has been deleted, don't insert into index; it also maybe 0.
	// repair curlevel so that it will be deleted at level l.  no other core will
	// increase it anymore, because e is marked for deletion
	__sync_fetch_and_sub(&(e->curlevel), 1);
	goto done;
      }
      assert (n == 0);
    } while (!__sync_bool_compare_and_swap(&(e->next[l+1]), 0, s));
    if (__sync_bool_compare_and_swap(&(p1->next[l+1]), s, e)) {  // insert in list l+1
      ; // pr[l+2] = e;   // XXX adding e on a level it shouldn't be?
    } else {
      (void) __sync_fetch_and_and(&(e->next[l+1]), 0x1);    // keep mark bit
      __sync_fetch_and_sub(&(e->curlevel), 1);
      // cprintf("%d: crange_add_index: retry add level %d %u(%u)\n", mycpu()->id, l+1, e->key, e->size);
    }
  }
 done:
  crange_check(cr, 0, NULL);
}

// Given the node that starts the range, find all other nodes part of range and lock them,
// if l == 0
static int
crange_lock_range(u64 k, u64 sz, int l, struct clist_range **er, struct clist_range **pr, 
		  struct clist_range **fr, struct clist_range **lr, struct clist_range **sr)
{
  struct clist_range *e = *er;
  assert(*pr != e);
  *fr = e;
  *lr = e;
  if (l == 0) {
    // lock p, if still pointing to e (at the bottom level)
    if (!crange_lockpred(*pr, e)) 
      return 0;
    // locked p and e; we are in business
  }
  // find successor of [k, sz)
  while (e && (k+sz) > e->key) {
    assert(*fr);
    *lr = e;
    if (l == 0) {
      acquire(e->lock);    // lock all nodes in the range
    }
    e = RANGE_WOMARK(e->next[l]);
  }
  *sr = e;
  *er = e;
  return 1;
}

// finds and locks all nodes in range [k, sz).  Also, returns predecessors
// and successors.  Locks pred and nodes in bottom list. If crange_lockpred() fails, we search
// again.
static int
crange_find_and_lock(struct crange *cr, u64 k, u64 sz, struct clist_range **p0, 
		     struct clist_range **f0, struct clist_range **l0, struct clist_range **s0)
{
  // need only current and last;
  struct clist_range *p1, *s1;
  struct clist_range *e;
 retry:
  *p0 = NULL;
  *s0 = NULL;
  for (int l = cr->nlevel-1; l >= 0; l--) {  
    *f0 = NULL;
    *l0 = NULL;
    p1 = *p0;
    *p0 = (l == cr->nlevel-1) ? &cr->crange_head : p1;
    s1 = *s0;
    for (e = RANGE_WOMARK((*p0)->next[l]); e; *p0 = e, e = RANGE_WOMARK(e->next[l])) {
      assert(!MARKED(e));
      assert(l < e->nlevel);
      int r = crange_del_index(cr, *p0, &e, l);
      if (r == -1) goto retry;   // deletion failed because some other core did it; try again
      if (r == 0) continue;  // node was marked but we are level 0, skip it; lock holder will remove
      if (e == 0) break;   // all nodes on this level were removed
      if (k >= e->key+e->size)  { // is e before k? 
	crange_add_index(cr, l, e, p1, s1);    // maybe add to index
	continue;
      }
      if (crange_intersect(k, sz, e->key, e->size)) {   // first node of range?
	if (!crange_lock_range(k, sz, l, &e, p0, f0, l0, s0)) {
	  goto retry;
	}
      } else {  // not present on this level; e succeeds [k, sz)
	*s0 = e;
      }
      assert(!e || e->nlevel > l);
      break;
    }
  }
  if (*f0 == NULL) {   // range isn't present, lock predecessor of key
    if (!crange_lockpred(*p0, *s0)) goto retry;
  }
  assert(!*f0 || ((*f0)->lock->locked));
  assert(!*l0 || ((*l0)->lock->locked));
  assert(!*l0 || !MARKED((*l0)->next[0]));
  assert(!MARKED((*p0)->next));
  assert(!(*p0)->next[0] || !MARKED((*p0)->next[0]->next[0]));
  // cprintf("find(%d,%d): ", k, sz); crange_print(1);
  return *f0 != 0;
}

// Compute the sublist that will replace the to-be deleted range. Make copies to create
// the new nodes, because readers may running through the list and looking at the old nodes.
static struct clist_range *
crange_replace(u64 k, u64 sz, void *v, struct clist_range *f, struct clist_range *l, 
	       struct clist_range *s)
{
  struct clist_range *r;
  
  assert(!f || f->lock->locked);
  assert(!l || l->lock->locked);

  if (f == l) {  // the first node covers range to be deleted
    if (k <= f->key && f->key + f->size <= k + sz) {  // range covers the first node
      r = s;
    } else {
      if (f->key < k && k+sz < f->key + f->size) { 	  // split node?
	struct clist_range *right = crange_new(f->cr, k+sz, f->key+f->size-k-sz, v, s);
	struct clist_range *left = crange_new(f->cr, f->key, k-f->key, v, right);
	r = left;
      } else if (k <= f->key) { // cut front?
	assert(k+sz <= f->key + f->size);
	r = crange_new(f->cr, k+sz, f->key + f->size - k - sz, v, f->next[0]);
      } else {  // cut end
	assert(k > f->key);
	r = crange_new(f->cr, f->key, k - f->key, v, f->next[0]);
      }
    }
  } else if (k <= f->key && k + sz >= l->key + l->size) {  // delete complete range?
    r = s;
  } else {  // first node covers part and last node other part?
    struct clist_range *left;
    struct clist_range *right;
    // cprintf("f 0x%lx [%d, %d) l 0x%lx [%d, %d)\n", (long) f, f->key, f->size, (long) l, l->key, l->size);
    if (k <= f->key && k + sz >= f->key + f->size) {  // delete first node?
      left = NULL;
    } else {
      assert(k > f->key);
      left = crange_new(f->cr, f->key, k - f->key, v, 0);
    }
    if (k + sz >= l->key + l->size)  { // delete last node?
      right = NULL;
    } else {
      assert(k+sz > l->key);
      assert(l->key + l->size >= k + sz);
      right = crange_new(f->cr, k+sz, l->key+l->size - k - sz, v, s);
    }
    r = left ? left : right;
    if (left) left->next[0] = right ? right : s;
  }
  return r;
}

// A node is marked for deletion when its next pointer has been marked; don't follow those
struct clist_range*
crange_search(struct crange *cr, u64 k)
{
  struct clist_range *p, *e, *r;
  gc_begin_epoch();
  if (crange_debug) cprintf("crange_search: 0x%lx 0x%lx\n", (u64) cr, k);
  r = NULL;
  p = &cr->crange_head;
  for (int l = cr->nlevel-1; l >= 0; l--) {
    for (e = RANGE_WOMARK(p->next[l]); e; p = e, e = RANGE_WOMARK(e->next[l])) {
      if (MARKED(e->next[l])) {   // drop down a level?  what if p's next is marked?
	p = &cr->crange_head;
	break;
      }
      if (k >= e->key+e->size)
	continue;
      if (e->key <= k && k < e->key + e->size) {
	r = e;
	goto end;
      } 
      // search on this level failed, drop a level. but continue at predecessor
      break;
    }
  }
 end:
  gc_end_epoch();
  // cprintf("crange_search: 0x%x return (0x%lx,0x%lx)\n", cr, r? r->key : 0, r? r->size : 0);
  return r;
}

// delete the range [k, k+sz).  We compute the replacement list and then hook it in atomically.
void
crange_del(struct crange *cr, u64 k, u64 sz)
{
  struct clist_range *prev;
  struct clist_range *succ;
  struct clist_range *first;
  struct clist_range *last;
  struct clist_range *repl = NULL;

  if (crange_debug) cprintf("crange_del: 0x%lx (0x%lx,0x%lx)\n", (u64) cr, k, sz);
  assert(cr);
  gc_begin_epoch();
  if (!crange_find_and_lock(cr, k, sz, &prev, &first, &last, &succ)) { // done?
    if (crange_debug) cprintf("crange_del: [0x%lx,0x%lx) not present\n", k, sz);
    release(prev->lock);
    goto done;
  }
  repl = crange_replace(k, sz, NULL, first, last, succ); // XXX cannot be NULL
  crange_mark(first, succ, 0); // mark first till s on level >= 0
  while (1) {
    // hook new list into bottom list; if del resulted in a new list, use that (repl), otherwise
    // set predecessor to successor.
    if (__sync_bool_compare_and_swap(&(prev->next[0]), first, repl ? repl : succ)) {
      release(prev->lock);
      crange_freen(first, last);   // put on delayed list before unlocking
      crange_unlockn(first, last);
      break;
    }
    cprintf("crange_del(%lu, %lu): prev was updated; try again\n", k, sz);
    assert(0);
  }
 done:
  crange_check(cr, 1, NULL);
  // cprintf("%d: crange_del(0x%lx, 0x%lx):\n", mycpu()->id, k, sz);  crange_print(cr, 1);
  gc_end_epoch();
}


// add the range [k, sz), which causes nodes to be deleted, if the range overlaps an 
// existing range.  we compute the replacement list and then hook it atomically.
void 
crange_add(struct crange *cr, u64 k, u64 sz, void *v)
{
  struct clist_range *new;
  struct clist_range *first;
  struct clist_range *prev;
  struct clist_range *last;
  struct clist_range *succ;
  struct clist_range *repl = NULL;

  if (crange_debug) cprintf("crange_add: 0x%lx (0x%lx,0x%lx)\n", (u64) cr, k, sz);
  assert(cr);
  gc_begin_epoch();
  if (crange_find_and_lock(cr, k, sz, &prev, &first, &last, &succ)) {
    if (crange_debug) cprintf("crange_add(0x%lx,0x%lx) overlaps with [0x%lx,0x%lx)\n", k, sz, first->key, first->size);
    repl = crange_replace(k, sz, v, first, last, succ);
  } else {
    repl = succ;
  }
  new = crange_new(cr, k, sz, v, succ);
  repl = crange_insert(repl, new);
  crange_mark(first, succ, 0); // mark first till s on all levels
  if (prev)
    assert(!MARKED(prev->next[0]));
  if (__sync_bool_compare_and_swap(&(prev->next[0]), first ? first : succ, repl)) {
    release(prev->lock);
    crange_freen(first, last);   // put on delayed list before unlocking
    crange_unlockn(first, last);
  } else {
    assert(0);
  }
  // cprintf("crange_add(0x%lx,0x%lx):\n", k, sz);  crange_print(cr, 1);
  crange_check(cr, 1, NULL);
  gc_end_epoch();
}

int
crange_foreach(struct crange *cr, int (*cb)(struct clist_range *r, void *), void *st)
{
  struct clist_range *e;
  assert(cr);
  for (e = RANGE_WOMARK(cr->crange_head.next[0]); e; e = RANGE_WOMARK(e->next[0])) {
    if (!cb(e, st))
      return 0;
  }
  return 1;
}
