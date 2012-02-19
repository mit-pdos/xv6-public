#include "types.h"
#include "kernel.hh"
#include "mmu.h"
#include "spinlock.h"
#include "condvar.h"
#include "queue.h"
#include "proc.hh"
#include "cpu.hh"
#include "gc.hh"
#include "crange.hh"
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

enum { crange_debug = 0 };
enum { crange_checking = 0 };

#define dprintf(...) do { if (crange_debug) cprintf(__VA_ARGS__); } while(0)

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

void range::print(int l)
{
  cprintf ("0x%lx-0x%lx(%lu) 0x%lx, c %d, t %d, n 0x%lx m %d\n",
           key, key+size, size, (long) value, curlevel.load(), nlevel,
           (long) next, (bool) next[l].mark());
}

range::~range()
{
  dprintf("%d: range_free: 0x%lx 0x%lx-0x%lx(%ld)\n", myproc()->cpuid, (u64) this, key, key+size, size);
  cr->check(this);
  //    assert(curlevel == -1);
  for (int l = 0; l < nlevel; l++) {
    next[l] = (struct range *) 0xDEADBEEF;
  }
  kmalignfree(lock);
  kmfree(next);
}

void range::dec_ref(void)
{
  int n = curlevel--;
  if (n == 0) {    // now removed from all levels.
    dprintf("%d: free_delayed: 0x%lx 0x%lx-0x%lx(%lu) %lu\n", myproc()->pid, (long) this, key, key + size, size, myproc()->epoch);
    cr->check(this);
    assert(curlevel == -1);
    gc_delayed(this);
  }
}

range::range(crange *crarg, u64 k, u64 sz, void *v, range *n, int nl)
  : rcu_freed("range_delayed")
{
  dprintf("range:range:: %lu %lu %d\n", k, sz, nl);
  key = k;
  size = sz;
  value = v;
  cr = crarg;
  assert(cr->nlevel > 0);
  curlevel = 0;
  if (nl == 0) nlevel = range_draw_nlevel(cr->nlevel);
  else nlevel = nl;
  next = new markptr<range>[nlevel]; // cache align?
  assert(next);
  next[0] = n;
  for (int l = 1; l < nlevel; l++) next[l] = 0;
  assert(kmalign((void **) &lock, CACHELINE, 
			   sizeof(struct spinlock)) == 0);
  initlock(lock, "crange", LOCKSTAT_CRANGE);
}

// 
// Methods on a sequence (i.e., ordered list) of ranges
//

static range *insert(struct range *head, struct range *r)
{
  markptr<range> n, p;
  p = nullptr;
  for (n = head; n != 0; p = n, n = n->next[0]) {
    assert(!n.mark());
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

// lock p if next == e and p isn't marked for deletion.  if not, return failure.
int range::lockif(markptr<range> e)
{
  assert(!e.mark());
  acquire(lock);
  if (next[0] == e) {
      return 1;
  } 
  release(lock);
  // cprintf("%d: range_lock_pred: retry %u\n", mycpu()->id, key);
  return 0;
}

// Mark ranges f till s for deletion from top-level down through level 0.
// It must be top-down to ensure that del_index/add_index don't race on
// curlevel.  If bottom-up, del_index may delete a node at a low level,
// causing curlevel to drop below nlevel, and causing add_index to add the
// node back on level on which it already has been inserted (because it hasn't
// been marked deleted yet at that level).
static void mark(range *f, range *s)
{
  struct range *e;
  for (e = f; e && e != s; e = e->next[0].ptr()) {
    assert(e);
    for (int l = e->nlevel-1; l >= 0; l--)
      e->next[l].mark() = 1;
  }
}

// Unlock ranges f through l
static void unlockn(range *f, range *l)
{
  struct range *e;
  for (e = f; e != l; e = e->next[0].ptr()) {
    assert(e);
    release(e->lock);
  }
  if (l) release(e->lock);
}

// Delay free ranges f through l
static void freen(struct range *f, struct range *l)
{
  struct range *e;
  for (e = f; e != l; e = e->next[0].ptr()) {
    assert(e);
    assert(e->curlevel >= 0);
    e->dec_ref();
  }
  if (l) {
    assert(e->curlevel >= 0);
    e->dec_ref();
  }
}

// Compute the sequence that will replace the to-be deleted sequence. Make copies to create
// the new ranges, because readers may running through the list and looking at the old ranges.
// If the whole sequence is replaced, it will return s.
static range *replace(u64 k, u64 sz, void *v, range *f, range *l, range *s)
{
  range *r;
  
  if (f == l) {  // the first range covers the range to be deleted
    if (k <= f->key && f->key + f->size <= k + sz) {  // range sequence covers the first range
      r = s;
    } else {
      if (f->key < k && k+sz < f->key + f->size) { 	  // split range?
	range *right = new range(f->cr, k+sz, f->key+f->size-k-sz, v, s);
	range *left = new range(f->cr, f->key, k-f->key, v, right);
	r = left;
      } else if (k <= f->key) { // cut front?
	assert(k+sz <= f->key + f->size);
        assert(!f->next[0].mark());
	r = new range(f->cr, k+sz, f->key + f->size - k - sz, v, f->next[0].ptr());
      } else {  // cut end
	assert(k > f->key);
        assert(!f->next[0].mark());
	r = new range(f->cr, f->key, k - f->key, v, f->next[0].ptr());
      }
    }
  } else if (k <= f->key && k + sz >= l->key + l->size) {  // delete complete range?
    r = s;
  } else {  // first range covers part and last range other part?
    range *left;
    range *right;
    // cprintf("f 0x%lx [%d, %d) l 0x%lx [%d, %d)\n", (long) f, f->key, f->key+f->size, (long) l, l->key, l->key+l->size);
    if (k <= f->key && k + sz >= f->key + f->size) {  // delete first range?
      left = nullptr;
    } else {
      assert(k > f->key);
      left = new range(f->cr, f->key, k - f->key, v, 0);
    }
    if (k + sz >= l->key + l->size)  { // delete last range?
      right = nullptr;
    } else {
      assert(k+sz > l->key);
      assert(l->key + l->size >= k + sz);
      right = new range(f->cr, k+sz, l->key+l->size - k - sz, v, s);
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
crange::print(int full)
{
  for (int l = 0; l < nlevel; l++) {
    int c = 0;
    cprintf("crange %d: ", l);
    for (range *e = crange_head->next[l].ptr(); e != 0; e = e->next[l].ptr()) {
      c++;
      if (full) e->print(l);
    }
    cprintf(" cnt %d \n", c);
  }
}

crange::crange(int nl)
{
  assert(nl > 0);
  nlevel = nl;
  crange_head = new range(this, 0, 0, nullptr, nullptr, nlevel);
  dprintf("crange::crange return 0x%lx\n", (u64) this);
}

crange::~crange()
{
  dprintf("crange_free: 0x%lx\n", (u64) this);
  range *e, *n;
  for (e = crange_head->next[0].ptr(); e; e = n) {
    n = e->next[0].ptr();
    delete e;
  }
  delete crange_head;
}

// Check some invariants, ignoring marked nodes.
void crange::check(struct range *absent)
{
  if (!crange_checking)
    return;
  int t = mycpu()->id;
  struct range *e, *s;
  for (int l = 0; l < nlevel; l++) {
    for (e = crange_head->next[l].ptr(); e; e = s) {
      assert(e->curlevel < nlevel);
      if (absent == e)  {
	cprintf("%d: check level failed; 0x%lx is present\n", l, (u64) absent);
	assert(0);
      }
      // look for e level down, but only for non-marked nodes.
      if (l > 0 && e->next[l] != 0 && !e->next[l].mark()) {
	struct range *n;
	for (n = crange_head->next[l-1].ptr(); n && n != e; n = n->next[l-1].ptr())
	  ;
	__sync_synchronize();
	// if e is marked now, skip the check (the memory barrier ensures that we reread it
	// from memory (and not from a register)
	if (!e->next[l].mark() && n != e) {
	  cprintf("%d: check level %d failed 0x%lx-0x%lx(%lu) m %d c %d t %d; in high level but not low\n", t, l, e->key, e->key+e->size, e->size, (bool)e->next[l].mark(), e->curlevel.load(), e->nlevel);
	  print(1);
	  assert(0);
	}
      }
      // check if succ range is after n
      s = e->next[l].ptr();
      assert(s != e);
      if (!e->next[l].mark() && s && (e->key + e->size > s->key)) {
	dprintf("%d: e(%lu,%lu) overlaps with s(%lu,%lu)\n", t, e->key, e->size, s->key, e->size);
	print(1);
	assert(0);
      }
    }
  }
}

// Remove e from index, if marked for deletion.  Returns 1 if e isn't marked.
// Returns 0, if marked but on level 0. Returns -1 if remove fails.
// Returns 1 on success.  Tricky because of races between add and del.
int crange::del_index(range *p0, range **e, int l)
{
  int r = 1;
  assert(l < (*e)->nlevel);
  if (!(*e)->next[l].mark())  // don't remove unmarked ranges from index
    return r;
  if (l == 0) return 0;    // but not on level 0; they are locked when removed
  // crange_check(cr, NULL);
  while (*e && (*e)->next[l].mark()) {
#if 0
    if (l != (*e)->curlevel) {
      // range is still in the index one level up, back out.  we want to remove it first 
      // at higher levels so that we ensure the invariant ranges are removed top down.
      // cprintf("%d: crange_del_index: retry del %d %u(%u) on level %d\n", mycpu()->id, l, (*e)->key, (*e)->size,  (*e)->curlevel);
      r = -1;
      goto done;
    }
#endif
    bool cas = p0->next[l].cmpxch(*e, (*e)->next[l].ptr().load());
    if (cas) {
      assert((*e)->curlevel >= 0);
      (*e)->dec_ref();
      *e = (*e)->next[l].ptr();
    } else {  
      // cprintf("%d: crange_del_index: retry del %u(%u)\n", mycpu()->id, (*e)->key, (*e)->key + (*e)->size);
      r = -1;
      //INCRETRY;
      goto done;
    }
  }
 done:
  check(nullptr);
  return r;
}

// Insert e into index one level up, between p and s, if e hasn't been inserted
// yet on that level.
void crange::add_index(int l, range *e, range *p1, markptr<range> s1)
{
  if (l >= e->nlevel-1) return;
  if (e->next[l+1].mark()) return;
  // crange_check(cr, NULL);
  if (cmpxch(&e->curlevel, l, l+1)) {
    assert(e->curlevel < e->nlevel);
    // this is the core inserting at level l+1, but some core may be deleting
    struct range *s = s1.ptr(); // XXX losing the mark bit ???
    do {
      markptr<range> n = e->next[l+1];   // Null and perhaps marked
      if (n.mark()) {
	// this range has been deleted, don't insert into index.
	// undo increment of cur->level.
	e->curlevel--;
	//INCRETRY;
	goto done;
      }
      assert (n == 0);
    } while (!e->next[l+1].cmpxch(0, s));
    if (!p1->next[l+1].cmpxch(s, e)) {  // insert in list l+1
      e->next[l+1].ptr() = 0;    // failed, keep mark bit
      e->curlevel--;
      // cprintf("%d: crange_add_index: retry add level %d %u(%u)\n", mycpu()->id, l+1, e->key, e->key+e->size);
      //INCRETRY;
    }
  }

 done:
  check(nullptr);
}

// Given the range that starts the sequence, find all other ranges part of sequence and lock them,
// if l == 0
int crange::lock_range(u64 k, u64 sz, int l, range **er, range **pr, range **fr, range **lr, range **sr)
{
  struct range *e = *er;
  assert(*pr != e);
  *fr = e;
  *lr = e;
  if (l == 0) {
    // lock p, if still pointing to e (at the bottom level)
    if (!(*pr)->lockif(e)) 
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
    e = e->next[l].ptr();
  }
  *sr = e;
  *er = e;
  return 1;
}

// finds and locks all ranges in sequence [k, sz).  Also, returns predecessors
// and successors.  Locks pred and ranges in bottom list. If range_lock_pred()
// fails, search again.
int crange::find_and_lock(u64 k, u64 sz, range **p0, range **f0, range **l0, range **s0)
{
  struct range *p1, *s1;
  struct range *e;
  //read_counters(myproc()->cpuid, 0);
 retry:
  *p0 = nullptr;
  *s0 = nullptr;
  for (int l = nlevel-1; l >= 0; l--) {  
    *f0 = nullptr;
    *l0 = nullptr;
    p1 = *p0;   // remember last previous (p0) as the previous one level up (p1)
    *p0 = (l == nlevel-1) ? crange_head : p1;   // set current previous
    s1 = *s0;
    for (e = (*p0)->next[l].ptr(); e; *p0 = e, e = e->next[l].ptr()) {
      assert(l < e->nlevel);
      int r = del_index(*p0, &e, l);
      if (r == -1) goto retry;   // deletion failed because some other core did it; try again
      if (r == 0) continue;  // range was marked but we are level 0, skip it; lock holder will remove
      if (e == 0) break;   // all ranges on this level were removed
      if (k >= e->key+e->size)  { // is e before k? 
	add_index(l, e, p1, s1);    // maybe add to index
	continue;
      }
      if (range_intersect(k, sz, e->key, e->size)) {   // first range of sequence?
	if (!lock_range(k, sz, l, &e, p0, f0, l0, s0)) {
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
    if (!(*p0)->lockif(*s0)) {
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
range* crange::search(u64 k, u64 sz, int mod)
{
  struct range *p, *e, *r;
  int n = (mod) ?  range_draw_nlevel(nlevel) : 0;
  gc_begin_epoch();
  //read_counters(myproc()->cpuid, 0);
  dprintf("crange_search: 0x%lx 0x%lx\n", (u64) this, k);
  r = nullptr;
  p = crange_head;
  for (int l = nlevel-1; l >= 0; l--) {
    for (e = p->next[l].ptr(); e; p = e, e = e->next[l].ptr()) {
      dprintf("level %d: 0x%lx 0x%lx-%lx(%lu) 0x%lx-0x%lx(%lu)\n", l, (u64) p, p->key, p->key+p->size, p->size, e->key, e->key+e->size, e->size);
      // skip all marked ranges, but don't update p because
      // we don't want to descend on a marked range down.
      while (e && e->next[l].mark()) {
	e = e->next[l].ptr();
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
  dprintf("crange_search: 0x%lx return (0x%lx,0x%lx)\n", (u64) this, r? r->key : 0, r? r->size : 0);
  return r;
}

// delete the range [k, k+sz). compute the replacement list and then hook it in atomically.
void crange::del(u64 k, u64 sz)
{
  struct range *prev;
  struct range *succ;
  struct range *first;
  struct range *last;
  struct range *repl = nullptr;

  assert(this);
  gc_begin_epoch();
  dprintf("crange_del: 0x%lx 0x%lx-0x%lx(%ld)\n", (u64) this, k, k+sz, sz);
  if (!find_and_lock(k, sz, &prev, &first, &last, &succ)) { // done?
    dprintf("crange_del: [0x%lx,0x%lx) not present\n", k, sz);
    release(prev->lock);
    goto done;
  }
  repl = replace(k, sz, nullptr, first, last, succ);
  mark(first, succ); 
  while (1) {
    // hook new list into bottom list; if del resulted in a new list, use that (repl), otherwise
    // set predecessor to successor.
    if (prev->next[0].cmpxch(first, repl ? repl : succ)) {
      release(prev->lock);
      freen(first, last);   // put on delayed list before unlocking
      unlockn(first, last);
      break;
    }
    cprintf("crange_del(%lu, %lu): prev was updated; try again\n", k, sz);
    assert(0);
  }

 done:
  check(nullptr);
  // cprintf("%d: crange_del(0x%lx, 0x%lx):\n", mycpu()->id, k, sz);  crange_print(cr, 1);
  gc_end_epoch();
}


// add the range [k, sz), which causes ranges to be deleted, if the range overlaps an 
// existing range.  we compute the replacement list and then hook it atomically.
void crange::add(u64 k, u64 sz, void *v)
{
  struct range *r;
  struct range *first;
  struct range *prev;
  struct range *last;
  struct range *succ;
  struct range *repl = nullptr;

  dprintf("crange_add: 0x%lx 0x%lx-0x%lx(%lu)\n", (u64) this, k, k+sz, sz);
  assert(this);
  gc_begin_epoch();
  if (find_and_lock(k, sz, &prev, &first, &last, &succ)) {
    dprintf("crange_add(0x%lx,0x%lx) overlaps with [0x%lx,0x%lx)\n", k, sz, first->key, first->size);
    repl = replace(k, sz, v, first, last, succ);
  } else {
    repl = succ;
  }
  r = new range(this, k, sz, v, succ);
  repl = insert(repl, r);
  mark(first, succ);
  if (prev)
    assert(!prev->next[0].mark());
  if (prev->next[0].cmpxch(first ? first : succ, repl)) {
    release(prev->lock);
    freen(first, last);   // put on delayed list before unlocking
    unlockn(first, last);
  } else {
    assert(0);
  }
  // cprintf("crange_add(0x%lx,0x%lx):\n", k, sz);  crange_print(cr, 1);
  check(nullptr);
  gc_end_epoch();
}
