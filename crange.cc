extern "C" {
#include "types.h"
#include "kernel.h"
#include "mmu.h"
#include "spinlock.h"
#include "condvar.h"
#include "queue.h"
#include "proc.h"
#include "cpu.h"
}

#include "gc.hh"
#include "cpputil.hh"

//
// Concurrent atomic range operations using skip lists.  An insert may split an
// existing range in several ranges.  A delete may remove a sequence of ranges
// or split an existing range.  An insert with a concurrent delete occur
// atomically, which we accomplish by taking locks on the sequence of ranges
// that are involved in the operation, compute the replacement, and atomically
// swich the locked sequence with the replacement sequence.
//
// Overall implementation approach is that the bottom list (layer 0) contains
// the truth, and the index is an performance-accelerating hint.  Inserts lazily
// adds a range to the index, starting from layer 1, going up.  Delete marks
// the sequence atomically as deleted, but lazily removes it from layers [1,n],
// in any order.
//
// Searches run without locks, and ignore marked ranges (pretending that they are not
// part of the index).
//
// Changes to the bottom list acquire locks on the ranges in the effected sequence.
// Thus, if two changes are to different parts of the list, they can happen in
// parallel, but operations on overlapping sequences are serialized. The
// invariant for deletion and insertion is that we always lock the
// predecessor range, to avoid races.
// As with concurrent lists, the hard cases are: 1) a concurrent remove and
// insert, and 2) concurrent removes of two ranges in sequence. Re 1: the risk is
// that the insert updates the next pointer of a range to be removed.
// Locking the predecessors avoids these races.
//
// A detail is that when we found the ranges corresponding to a sequence, we
// must lock the predecessor first and then the first range of the sequence.  But,
// while waiting for the lock for the predecessor, another core may be deleting
// the predecessor. To handle this race, the core that deletes a range marks a
// range for deletion by marking its next pointer.
//
// After a delete of a range, the range is not put on the delayed-free list
// until it has been removed from the index (i.e., curlevel reaches 0), which
// maybe many epochs later.
//

#define MARKED(x) (((uptr) (x)) & 0x1)
#define MARK(x) ((struct range *) (((uptr) (x)) | 0x1))
#define WOMARK(x) ((struct range *) (((uptr) (x)) & ~0x1))

enum { crange_debug = 0 };
enum { crange_checking = 0 };

void crange_check(struct crange *cr, struct range *absent);

//
// Methods for ranges
//

// does [k1, sz1) intersect with [k2, sz2)?
static int 
range_intersect(u64 k1, u64 sz1, u64 k2, u64 sz2)
{
  if (k1+sz1 <= k2) return 0;  // region1 < region2
  else if (k1 >= k2+sz2) return 0;  // region1  > region2
  else return 1;
}

// draw nlevel in [1, nlevel]
static int
range_draw_nlevel(int nlevel)
{
  int l;
  for (l = 0; l < nlevel-1; l++) {
    if (rnd() % 16777216 > 8388608)    // % 1 is not enough randomness
      break;
  }
  return l+1;
}

void 
range_print(struct range *e, int l)
{
  cprintf ("0x%lx-0x%lx(%lu) 0x%lx, c %d, t %d, n 0x%lx m 0x%lx\n", e->key, e->key+e->size, e->size, (long) e->value, e->curlevel, e->nlevel, (long) e->next, MARKED(e->next[l]));
}

static void
range_free(void *p)
{
  struct range *e = (struct range *) p;
  if (crange_debug)
    cprintf("%d: range_free: 0x%lx 0x%lx-0x%lx(%ld)\n", myproc()->cpuid, (u64) e, e->key, e->key+e->size, e->size);
  crange_check(e->cr, e);
  assert(e->curlevel == -1);
  for (int l = 0; l < e->nlevel; l++) {
    e->next[l] = (struct range *) 0xDEADBEEF;
  }
  kmalignfree(e->lock);
  kmfree(e->next);
  kmalignfree(e);
}

void
range::do_gc()
{
  range_free(this);
}

static void
range_free_delayed(struct range *e)
{
  if (crange_debug)
    cprintf("%d: range_free_delayed: 0x%lx 0x%lx-0x%lx(%lu) %lu\n", myproc()->pid, (long) e, e->key, e->key + (e)->size, e->size, myproc()->epoch);
  crange_check(e->cr, e);
  assert(e->curlevel == -1);
  gc_delayed(e);
}

static void
range_dec_ref(struct range *e)
{
  int n = __sync_fetch_and_sub(&(e->curlevel), 1);
  if (n == 0) {    // now removed from all levels.
    range_free_delayed(e);
  }
}

static struct range *
range_alloc(struct crange *cr, u64 k, u64 sz, void *v, struct range *n)
{
  void *rangemem;
  kmalign(&rangemem, CACHELINE, sizeof(struct range));
  assert(rangemem);

  struct range *r = new(rangemem) range();
  r->key = k;
  r->size = sz;
  r->value = v;
  assert(r->size > 0);
  assert(cr->nlevel > 0);
  r->curlevel = 0;
  r->nlevel = range_draw_nlevel(cr->nlevel);
  r->next = (struct range**) kmalloc(sizeof(sizeof(r->next[0])) * r->nlevel); // cache align?
  assert(r->next);
  r->next[0] = n;
  for (int l = 1; l < r->nlevel; l++) r->next[l] = 0;
  assert(kmalign((void **) &r->lock, CACHELINE, 
			   sizeof(struct spinlock)) == 0);
  initlock(r->lock, "crange", LOCKSTAT_CRANGE);
  r->cr = cr;
  return r;
}

// 
// Methods on a sequence (i.e., ordered list) of ranges
//

static struct range *
range_insert(struct range *head, struct range *r)
{
  struct range *n, *p;
  p = nullptr;
  for (n = head; n != 0; p = n, n = n->next[0]) {
    assert(!MARKED(n));
    if (r->key < n->key) {
      break;
    }
  }
  if (n == head) {
    r->next[0] = head;
    head = r;
  } else {
    p->next[0] = r;
    r->next[0] = n;
  }
  return head;
}

// lock p if p->next == e and p isn't marked for deletion.  if not, return failure.
static int
range_lock_pred(struct range *p, struct range *e)
{
  assert(!MARKED(e));
  acquire(p->lock);
  if (p->next[0] == e) {
      return 1;
  } 
  release(p->lock);
  // cprintf("%d: range_lock_pred: retry %u\n", mycpu()->id, p->key);
  return 0;
}

// Mark ranges f till s for deletion from top-level down through level 0.
// It must be top-down to ensure that del_index/add_index don't race on
// curlevel.  If bottom-up, del_index may delete a node at a low level,
// causing curlevel to drop below nlevel, and causing add_index to add the
// node back on level on which it already has been inserted (because it hasn't
// been marked deleted yet at that level).
static void 
range_mark(struct range *f, struct range *s)
{
  struct range *e;
  for (e = f; e && e != s; e = WOMARK(e->next[0])) {
    assert(e);
    for (int l = e->nlevel-1; l >= 0; l--) {
        (void) __sync_fetch_and_or(&(e->next[l]), (struct range *)0x1);
    }
  }
}

// Unlock ranges f through l
static void 
range_unlockn(struct range *f, struct range *l)
{
  struct range *e;
  for (e = f; e != l; e = WOMARK(e->next[0])) {
    assert(e);
    release(e->lock);
  }
  if (l) release(e->lock);
}

// Delay free ranges f through l
static void 
range_freen(struct range *f, struct range *l)
{
  struct range *e;
  for (e = f; e != l; e = WOMARK(e->next[0])) {
    assert(e);
    assert(e->curlevel >= 0);
    range_dec_ref(e);
  }
  if (l) {
    assert(e->curlevel >= 0);
    range_dec_ref(e);
  }
}

// Compute the sequence that will replace the to-be deleted sequence. Make copies to create
// the new ranges, because readers may running through the list and looking at the old ranges.
// If the whole sequence is replaced, it will return s.
static struct range *
crange_replace(u64 k, u64 sz, void *v, struct range *f, struct range *l, 
	       struct range *s)
{
  struct range *r;
  
  if (f == l) {  // the first range covers the range to be deleted
    if (k <= f->key && f->key + f->size <= k + sz) {  // range sequence covers the first range
      r = s;
    } else {
      if (f->key < k && k+sz < f->key + f->size) { 	  // split range?
	struct range *right = range_alloc(f->cr, k+sz, f->key+f->size-k-sz, v, s);
	struct range *left = range_alloc(f->cr, f->key, k-f->key, v, right);
	r = left;
      } else if (k <= f->key) { // cut front?
	assert(k+sz <= f->key + f->size);
	r = range_alloc(f->cr, k+sz, f->key + f->size - k - sz, v, f->next[0]);
      } else {  // cut end
	assert(k > f->key);
	r = range_alloc(f->cr, f->key, k - f->key, v, f->next[0]);
      }
    }
  } else if (k <= f->key && k + sz >= l->key + l->size) {  // delete complete range?
    r = s;
  } else {  // first range covers part and last range other part?
    struct range *left;
    struct range *right;
    // cprintf("f 0x%lx [%d, %d) l 0x%lx [%d, %d)\n", (long) f, f->key, f->key+f->size, (long) l, l->key, l->key+l->size);
    if (k <= f->key && k + sz >= f->key + f->size) {  // delete first range?
      left = nullptr;
    } else {
      assert(k > f->key);
      left = range_alloc(f->cr, f->key, k - f->key, v, 0);
    }
    if (k + sz >= l->key + l->size)  { // delete last range?
      right = nullptr;
    } else {
      assert(k+sz > l->key);
      assert(l->key + l->size >= k + sz);
      right = range_alloc(f->cr, k+sz, l->key+l->size - k - sz, v, s);
    }
    r = left ? left : right;
    if (left) left->next[0] = right ? right : s;
  }
  return r;
}

//
// Methods on a crange
//

void
crange_print(struct crange *cr, int full)
{
  struct range *e;
  for (int l = 0; l < cr->nlevel; l++) {
    int c = 0;
    cprintf("crange %d: ", l);
    for (e = cr->crange_head.next[l]; e; e = WOMARK(e->next[l])) {
      c++;
      if (full) range_print(e, l);
    }
    cprintf(" cnt %d \n", c);
  }
}

struct crange*
crange_alloc(int nlevel)
{
  struct crange *cr;
  assert(kmalign((void **) &cr, CACHELINE, sizeof(struct crange)) == 0);
  assert(nlevel >= 0);
  cr->nlevel = nlevel;
  cr->crange_head.cr = cr;
  cr->crange_head.key = 0;
  cr->crange_head.size = 0;
  assert(kmalign((void **) &cr->crange_head.lock, 
			   CACHELINE, sizeof(struct spinlock)) == 0);
  initlock(cr->crange_head.lock, "head lock", LOCKSTAT_CRANGE);
  cr->crange_head.next = (struct range **) kmalloc(sizeof(cr->crange_head.next[0]) * nlevel);
  for (int l = 0; l < nlevel; l++) cr->crange_head.next[l] = 0;
  if (crange_debug) cprintf("crange_alloc: return 0x%lx\n", (u64) cr);
  return cr;
}

void
crange_free(struct crange *cr)
{
  assert(cr);
  if (crange_debug) cprintf("crange_free: 0x%lx\n", (u64) cr);
  struct range *e, *n;
  for (e = WOMARK(cr->crange_head.next[0]); e; e = n) {
    n = WOMARK(e->next[0]);
    range_free(e);
  }
  kmfree(cr->crange_head.next);
  kmalignfree(cr->crange_head.lock);
  kmalignfree(cr);
}

// Check some invariants, ignoring marked nodes.
void
crange_check(struct crange *cr, struct range *absent)
{
  if (!crange_checking)
    return;
  int t = mycpu()->id;
  struct range *e, *s;
  for (int l = 0; l < cr->nlevel; l++) {
    for (e = cr->crange_head.next[l]; e; e = s) {
      assert(e->curlevel < cr->nlevel);
      if (absent == e)  {
	cprintf("%d: check level failed; 0x%lx is present\n", l, (u64) absent);
	assert(0);
      }
      // look for e level down, but only for non-marked nodes.
      if (l > 0 && e->next[l] != 0 && !MARKED(e->next[l])) {
	struct range *n;
	for (n = WOMARK(cr->crange_head.next[l-1]); n && n != e; n = WOMARK(n->next[l-1]))
	  ;
	__sync_synchronize();
	// if e is marked now, skip the check (the memory barrier ensures that we reread it
	// from memory (and not from a register)
	if (!MARKED(e->next[l]) && n != e) {
	  cprintf("%d: check level %d failed 0x%lx-0x%lx(%lu) m %lu c %d t %d; in high level but not low\n", t, l, e->key, e->key+e->size, e->size, MARKED(e->next[l]), e->curlevel, e->nlevel);
	  crange_print(cr, 1);
	  assert(0);
	}
      }
      // check if succ range is after n
      s = WOMARK(e->next[l]);
      assert(s != e);
      if (!MARKED(e->next[l]) && s && (e->key + e->size > s->key)) {
	if (crange_debug) cprintf("%d: e(%lu,%lu) overlaps with s(%lu,%lu)\n", t, e->key, e->size, s->key, e->size);
	crange_print(cr, 1);
	assert(0);
      }
    }
  }
}

// Remove e from index, if marked for deletion.  Returns 1 if e isn't marked.
// Returns 0, if marked but on level 0. Returns -1 if remove fails.
// Returns 1 on success.  Tricky because of races between add and del.
static int
crange_del_index(struct crange *cr, struct range *p0, struct range **e, int l)
{
  int r = 1;
  assert(l < (*e)->nlevel);
  if (!MARKED((*e)->next[l]))  // don't remove unmarked ranges from index
    return r;
  if (l == 0) return 0;    // but not on level 0; they are locked when removed
  // crange_check(cr, NULL);
  while (*e && MARKED((*e)->next[l])) {
#if 0
    if (l != (*e)->curlevel) {
      // range is still in the index one level up, back out.  we want to remove it first 
      // at higher levels so that we ensure the invariant ranges are removed top down.
      // cprintf("%d: crange_del_index: retry del %d %u(%u) on level %d\n", mycpu()->id, l, (*e)->key, (*e)->size,  (*e)->curlevel);
      r = -1;
      goto done;
    }
#endif
    int cas = __sync_bool_compare_and_swap(&(p0->next[l]), *e, WOMARK((*e)->next[l]));
    if (cas) {
      assert((*e)->curlevel >= 0);
      range_dec_ref(*e);
      *e = WOMARK((*e)->next[l]);
    } else {  
      // cprintf("%d: crange_del_index: retry del %u(%u)\n", mycpu()->id, (*e)->key, (*e)->key + (*e)->size);
      r = -1;
      //INCRETRY;
      goto done;
    }
  }
 done:
  crange_check(cr, nullptr);
  return r;
}

// Insert e into index one level up, between p and s, if e hasn't been inserted
// yet on that level.
static void
crange_add_index(struct crange *cr, int l, struct range *e, struct range *p1, struct range *s1)
{
  if (l >= e->nlevel-1) return;
  if (MARKED(e->next[l+1])) return;
  // crange_check(cr, NULL);
  if (__sync_bool_compare_and_swap(&e->curlevel, l, l+1)) {
    assert(e->curlevel < e->nlevel);
    // this is the core inserting at level l+1, but some core may be deleting
    struct range *s = WOMARK(s1);
    do {
      struct range *n = e->next[l+1];   // Null and perhaps marked
      if (MARKED(n)) {
	// this range has been deleted, don't insert into index.
	// undo increment of cur->level.
	__sync_fetch_and_sub(&(e->curlevel), 1);
	//INCRETRY;
	goto done;
      }
      assert (n == 0);
    } while (!__sync_bool_compare_and_swap(&(e->next[l+1]), 0, s));
    if (!__sync_bool_compare_and_swap(&(p1->next[l+1]), s, e)) {  // insert in list l+1
        (void) __sync_fetch_and_and(&(e->next[l+1]), (struct range *)0x1);    // failed, keep mark bit
      __sync_fetch_and_sub(&(e->curlevel), 1);
      // cprintf("%d: crange_add_index: retry add level %d %u(%u)\n", mycpu()->id, l+1, e->key, e->key+e->size);
      //INCRETRY;
    }
  }
 done:
  crange_check(cr, nullptr);
}

// Given the range that starts the sequence, find all other ranges part of sequence and lock them,
// if l == 0
static int
crange_lock_range(u64 k, u64 sz, int l, struct range **er, struct range **pr, 
		  struct range **fr, struct range **lr, struct range **sr)
{
  struct range *e = *er;
  assert(*pr != e);
  *fr = e;
  *lr = e;
  if (l == 0) {
    // lock p, if still pointing to e (at the bottom level)
    if (!range_lock_pred(*pr, e)) 
      return 0;
    // locked p and e; we are in business
  }
  // find successor of [k, sz)
  while (e && (k+sz) > e->key) {
    assert(*fr);
    *lr = e;
    if (l == 0) {
      acquire(e->lock);    // lock all ranges in the sequence
    }
    e = WOMARK(e->next[l]);
  }
  *sr = e;
  *er = e;
  return 1;
}

// finds and locks all ranges in sequence [k, sz).  Also, returns predecessors
// and successors.  Locks pred and ranges in bottom list. If range_lock_pred()
// fails, search again.
static int
crange_find_and_lock(struct crange *cr, u64 k, u64 sz, struct range **p0, 
		     struct range **f0, struct range **l0, struct range **s0)
{
  struct range *p1, *s1;
  struct range *e;
  //read_counters(myproc()->cpuid, 0);
 retry:
  *p0 = nullptr;
  *s0 = nullptr;
  for (int l = cr->nlevel-1; l >= 0; l--) {  
    *f0 = nullptr;
    *l0 = nullptr;
    p1 = *p0;   // remember last previous (p0) as the previous one level up (p1)
    *p0 = (l == cr->nlevel-1) ? &cr->crange_head : p1;   // set current previous
    s1 = *s0;
    for (e = WOMARK((*p0)->next[l]); e; *p0 = e, e = WOMARK(e->next[l])) {
      assert(l < e->nlevel);
      int r = crange_del_index(cr, *p0, &e, l);
      if (r == -1) goto retry;   // deletion failed because some other core did it; try again
      if (r == 0) continue;  // range was marked but we are level 0, skip it; lock holder will remove
      if (e == 0) break;   // all ranges on this level were removed
      if (k >= e->key+e->size)  { // is e before k? 
	crange_add_index(cr, l, e, p1, s1);    // maybe add to index
	continue;
      }
      if (range_intersect(k, sz, e->key, e->size)) {   // first range of sequence?
	if (!crange_lock_range(k, sz, l, &e, p0, f0, l0, s0)) {
	  //  INCRETRY;
	  goto retry;
	}
      } else {  // not present on this level; e succeeds [k, sz)
	*s0 = e;
      }
      assert(!e || e->nlevel > l);
      break;
    }
  }
  if (*f0 == nullptr) {   // range isn't present, lock predecessor of key
    if (!range_lock_pred(*p0, *s0)) {
      //INCRETRY;
      goto retry;
    }
  }
  //assert(!*l0 || !MARKED((*l0)->next[0]));
  //assert(!MARKED((*p0)->next));
  //assert(!(*p0)->next[0] || !MARKED((*p0)->next[0]->next[0]));
  //cprintf("find(%d,%d): ", k, sz); crange_print(1);
  //read_counters(myproc()->cpuid, 1);
  return *f0 != 0;
}


// Search through the crange skip list for a range that intersects with [k, sz)
// return that range. Pretend that marked ranges don't exist.
struct range*
crange_search(struct crange *cr, u64 k, u64 sz, int mod)
{
  struct range *p, *e, *r;
  int n = (mod) ?  range_draw_nlevel(cr->nlevel) : 0;
  gc_begin_epoch();
  //read_counters(myproc()->cpuid, 0);
  if (crange_debug) cprintf("crange_search: 0x%lx 0x%lx\n", (u64) cr, k);
  r = nullptr;
  p = &cr->crange_head;
  for (int l = cr->nlevel-1; l >= 0; l--) {
    for (e = WOMARK(p->next[l]); e; p = e, e = WOMARK(e->next[l])) {
      if (crange_debug)
	cprintf("level %d: 0x%lx 0x%lx-%lx(%lu) 0x%lx-0x%lx(%lu)\n", l, (u64) p, p->key, p->key+p->size, p->size, e->key, e->key+e->size, e->size);
      // skip all marked ranges, but don't update p because
      // we don't want to descend on a marked range down.
      while (e && MARKED(e->next[l])) {
	e = WOMARK(e->next[l]);
      }
      if (!e) break;
      if (mod && l < n && l > 0) {
	e->value = (void *) k;
      }
      if (k >= e->key+e->size)
	continue;
      if (range_intersect(k, sz, e->key, e->size)) {
	r = e;
	goto end;
      }
      // search on this level failed, drop a level. but continue at predecessor
      break;
    }
  }
 end:
  //read_counters(myproc()->cpuid, 1);
  gc_end_epoch();
  // cprintf("crange_search: 0x%x return (0x%lx,0x%lx)\n", cr, r? r->key : 0, r? r->size : 0);
  return r;
}

// delete the range [k, k+sz). compute the replacement list and then hook it in atomically.
void
crange_del(struct crange *cr, u64 k, u64 sz)
{
  struct range *prev;
  struct range *succ;
  struct range *first;
  struct range *last;
  struct range *repl = nullptr;

  assert(cr);
  gc_begin_epoch();
  if (crange_debug) 
    cprintf("crange_del: 0x%lx 0x%lx-0x%lx(%ld)\n", (u64) cr, k, k+sz, sz);
  if (!crange_find_and_lock(cr, k, sz, &prev, &first, &last, &succ)) { // done?
    if (crange_debug) cprintf("crange_del: [0x%lx,0x%lx) not present\n", k, sz);
    release(prev->lock);
    goto done;
  }
  repl = crange_replace(k, sz, nullptr, first, last, succ);
  range_mark(first, succ); 
  while (1) {
    // hook new list into bottom list; if del resulted in a new list, use that (repl), otherwise
    // set predecessor to successor.
    if (__sync_bool_compare_and_swap(&(prev->next[0]), first, repl ? repl : succ)) {
      release(prev->lock);
      range_freen(first, last);   // put on delayed list before unlocking
      range_unlockn(first, last);
      break;
    }
    cprintf("crange_del(%lu, %lu): prev was updated; try again\n", k, sz);
    assert(0);
  }
 done:
  crange_check(cr, nullptr);
  // cprintf("%d: crange_del(0x%lx, 0x%lx):\n", mycpu()->id, k, sz);  crange_print(cr, 1);
  gc_end_epoch();
}


// add the range [k, sz), which causes ranges to be deleted, if the range overlaps an 
// existing range.  we compute the replacement list and then hook it atomically.
void 
crange_add(struct crange *cr, u64 k, u64 sz, void *v)
{
  struct range *r;
  struct range *first;
  struct range *prev;
  struct range *last;
  struct range *succ;
  struct range *repl = nullptr;

  if (crange_debug) cprintf("crange_add: 0x%lx 0x%lx-0x%lx(%lu)\n", (u64) cr, k, k+sz, sz);
  assert(cr);
  gc_begin_epoch();
  if (crange_find_and_lock(cr, k, sz, &prev, &first, &last, &succ)) {
    if (crange_debug) cprintf("crange_add(0x%lx,0x%lx) overlaps with [0x%lx,0x%lx)\n", k, sz, first->key, first->size);
    repl = crange_replace(k, sz, v, first, last, succ);
  } else {
    repl = succ;
  }
  r = range_alloc(cr, k, sz, v, succ);
  repl = range_insert(repl, r);
  range_mark(first, succ);
  if (prev)
    assert(!MARKED(prev->next[0]));
  if (__sync_bool_compare_and_swap(&(prev->next[0]), first ? first : succ, repl)) {
    release(prev->lock);
    range_freen(first, last);   // put on delayed list before unlocking
    range_unlockn(first, last);
  } else {
    assert(0);
  }
  // cprintf("crange_add(0x%lx,0x%lx):\n", k, sz);  crange_print(cr, 1);
  crange_check(cr, nullptr);
  gc_end_epoch();
}

int
crange_foreach(struct crange *cr, int (*cb)(struct range *r, void *), void *st)
{
  struct range *e;
  assert(cr);
  for (e = WOMARK(cr->crange_head.next[0]); e; e = WOMARK(e->next[0])) {
    if (!cb(e, st))
      return 0;
  }
  return 1;
}
