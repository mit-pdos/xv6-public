#include "types.h"
#include "amd64.h"
#include "mmu.h"
#include "cpu.hh"
#include "kernel.hh"
#include "bits.hh"
#include "spinlock.h"
#include "kalloc.h"
#include "queue.h"
#include "condvar.h"
#include "proc.hh"
#include "vm.hh"
#include "gc.hh"
#include "crange.hh"
#include "cpputil.hh"

enum { vm_debug = 0 };

/*
 * vmnode
 */

vmnode::vmnode(u64 npg, vmntype ntype)
  : npages(npg), ref(0), type(ntype), ip(0), offset(0), sz(0)
{
  if (npg > NELEM(page))
    panic("vmnode too big\n");
  memset(page, 0, sizeof(page));
  if (type == EAGER)
    assert(allocpg() == 0);
}

vmnode::~vmnode()
{
  for(u64 i = 0; i < npages; i++) {
    if (page[i]) {
      kfree(page[i]);
      page[i] = 0;
    }
  }
  if (ip) {
    iput(ip);
    ip = 0;
  }
}

void
vmnode::decref()
{
  if(--ref == 0)
    delete this;
}

int
vmnode::allocpg()
{
  for(u64 i = 0; i < npages; i++) {
    if((page[i] = kalloc()) == 0)
      return -1;
    memset((char *) page[i], 0, PGSIZE);
  }
  return 0;
}

vmnode *
vmnode::copy()
{
  vmnode *c = new vmnode(npages, type);
  if(c != 0) {
    c->type = type;
    if (type == ONDEMAND) {
      c->ip = idup(ip);
      c->offset = offset;
      c->sz = c->sz;
    } 
    if (page[0]) {   // If the first page is present, all of them are present
      if (c->allocpg() < 0) {
	cprintf("vmn_copy: out of memory\n");
	delete c;
	return 0;
      }
      for(u64 i = 0; i < npages; i++) {
	memmove(c->page[i], page[i], PGSIZE);
      }
    }
  }
  return c;
}

int
vmnode::demand_load()
{
  for (u64 i = 0; i < sz; i += PGSIZE) {
    char *p = page[i / PGSIZE];
    s64 n;
    if (sz - i < PGSIZE)
      n = sz - i;
    else
      n = PGSIZE;
    if (readi(ip, p, offset+i, n) != n)
      return -1;
  }
  return 0;
}

int
vmnode::load(inode *iparg, u64 offarg, u64 szarg)
{
  ip = iparg;
  offset = offarg;
  sz = szarg;

  if (type == ONDEMAND)
    return 0;
  return demand_load();
}

/*
 * vma
 */

vma::vma(vmap *vmap, uptr start, uptr end, enum vmatype vtype)
  : range(&vmap->cr, start, end-start),
    vma_start(start), vma_end(end), va_type(vtype), n(0)
{
  snprintf(lockname, sizeof(lockname), "vma:%p", this);
  initlock(&lock, lockname, LOCKSTAT_VM);
}

vma::~vma()
{
  if(n)
    n->decref();
  destroylock(&lock);
}

/*
 * vmap
 */

vmap::vmap()
  : cr(10), pml4(setupkvm()), kshared((char*) ksalloc(slab_kshared))
{
  snprintf(lockname, sizeof(lockname), "vmap:%p", this);
  initlock(&lock, lockname, LOCKSTAT_VM);

  ref = 1;
  alloc = 0;

  if (pml4 == 0) {
    cprintf("vmap_alloc: setupkvm out of memory\n");
    goto err;
  }

  if (kshared == NULL) {
    cprintf("vmap::vmap: kshared out of memory\n");
    goto err;
  }

  if (setupkshared(pml4, kshared)) {
    cprintf("vmap::vmap: setupkshared out of memory\n");
    goto err;
  }

  return;

 err:
  if (kshared)
    ksfree(slab_kshared, kshared);
  if (pml4)
    freevm(pml4);
  destroylock(&lock);
}

vmap::~vmap()
{
  if (kshared)
    ksfree(slab_kshared, kshared);
  if (pml4)
    freevm(pml4);
  alloc = 0;
  destroylock(&lock);
}

void
vmap::decref()
{
  if (--ref == 0)
    delete this;
}

bool
vmap::replace_vma(vma *a, vma *b)
{
  auto span = cr.search_lock(a->vma_start, a->vma_end - a->vma_start);
  if (a->deleted())
    return false;
  for (auto e: span)
    assert(a == e);
  span.replace(b);
  return true;
}

vmap*
vmap::copy(int share)
{
  vmap *nm = new vmap();
  if(nm == 0)
    return 0;

  scoped_acquire sa(&lock);
  // XXX how to construct a consistent copy?

  for (range *r: cr) {
    vma *e = (vma *) r;
    scoped_acquire sae(&e->lock);

    struct vma *ne = new vma(nm, e->vma_start, e->vma_end, share ? COW : PRIVATE);
    if (ne == 0)
      goto err;

    if (share) {
      ne->n = e->n;

      // if the original vma wasn't COW, replace it with a COW vma
      if (e->va_type != COW) {
        vma *repl = new vma(this, e->vma_start, e->vma_end, COW);
        repl->n = e->n;
        repl->n->ref++;

        replace_vma(e, repl);
        updatepages(pml4, e->vma_start, e->vma_end, [](atomic<pme_t>* p) {
            for (;;) {
              pme_t v = p->load();
              if (v & PTE_LOCK)
                continue;
              if (!(v & PTE_P) || !(v & PTE_U) || !(v & PTE_W) ||
                  cmpxch(p, v, PTE_ADDR(v) | PTE_P | PTE_U | PTE_COW))
                break;
            }
          });
      }
    } else {
      ne->n = e->n->copy();
    }

    if (ne->n == 0)
      goto err;

    ne->n->ref++;
    auto span = nm->cr.search_lock(ne->vma_start, ne->vma_end - ne->vma_start);
    for (auto x __attribute__((unused)): span)
      assert(0);  /* span must be empty */
    span.replace(ne);
  }

  if (share) {
    // XXX should really be tlbflush(), but callers hold some locks..
    lcr3(rcr3());  // Reload hardware page table
  }

  return nm;

 err:
  delete nm;
  return 0;
}

// Does any vma overlap start..start+len?
// If yes, return the vma pointer.
// If no, return 0.
// This code can't handle regions at the very end
// of the address space, e.g. 0xffffffff..0x0
// We key vma's by their end address.
vma *
vmap::lookup(uptr start, uptr len)
{
  if (start + len < start)
    panic("vmap::lookup bad len");

  range *r = cr.search(start, len);
  if (r != 0) {
    vma *e = (vma *) r;
    if (e->vma_end <= e->vma_start)
      panic("malformed va");
    if (e->vma_start < start+len && e->vma_end > start)
      return e;
  }

  return 0;
}

int
vmap::insert(vmnode *n, uptr vma_start)
{
  vma *e;

  {
    // new scope to release the search lock before tlbflush
    u64 len = n->npages * PGSIZE;
    auto span = cr.search_lock(vma_start, len);
    for (auto r: span) {
      vma *rvma = (vma*) r;
      cprintf("vmap::insert: overlap with 0x%lx--0x%lx\n", rvma->vma_start, rvma->vma_end);
      return -1;
    }

    // XXX handle overlaps

    e = new vma(this, vma_start, vma_start+len, PRIVATE);
    if (e == 0) {
      cprintf("vmap::insert: out of vmas\n");
      return -1;
    }

    e->n = n;
    n->ref++;
    span.replace(e);
  }

  updatepages(pml4, e->vma_start, e->vma_end, [](atomic<pme_t> *p) {
      for (;;) {
        pme_t v = p->load();
        if (v & PTE_LOCK)
          continue;
        if (cmpxch(p, v, (pme_t) 0))
          break;
      }
    });
  tlbflush();
  return 0;
}

int
vmap::remove(uptr vma_start, uptr len)
{
  {
    // new scope to release the search lock before tlbflush
    uptr vma_end = vma_start + len;

    auto span = cr.search_lock(vma_start, len);
    for (auto r: span) {
      vma *rvma = (vma*) r;
      if (rvma->vma_start < vma_start || rvma->vma_end > vma_end) {
        cprintf("vmap::remove: partial unmap not supported\n");
        return -1;
      }
    }

    // XXX handle partial unmap

    span.replace(0);
  }

  updatepages(pml4, vma_start, vma_start + len, [](atomic<pme_t> *p) {
      for (;;) {
        pme_t v = p->load();
        if (v & PTE_LOCK)
          continue;
        if (cmpxch(p, v, (pme_t) 0))
          break;
      }
    });
  tlbflush();
  return 0;
}

/*
 * pagefault handling code on vmap
 */

vma *
vmap::pagefault_ondemand(uptr va, vma *m, scoped_acquire *mlock)
{
  if (m->n->allocpg() < 0)
    panic("pagefault: couldn't allocate pages");
  mlock->release();
  if (m->n->demand_load() < 0)
    panic("pagefault: couldn't load");
  m = lookup(va, 1);
  if (!m)
    panic("pagefault_ondemand");
  mlock->acquire(&m->lock); // re-acquire lock on m
  return m;
}

int
vmap::pagefault_wcow(vma *m)
{
  // Always make a copy of n, even if this process has the only ref, 
  // because other processes may change ref count while this process 
  // is handling wcow.
  struct vmnode *nodecopy = m->n->copy();
  if (nodecopy == 0) {
    cprintf("pagefault_wcow: out of mem\n");
    return -1;
  }

  vma *repl = new vma(this, m->vma_start, m->vma_end, PRIVATE);
  repl->n = nodecopy;
  nodecopy->ref = 1;

  replace_vma(m, repl);
  updatepages(pml4, m->vma_start, m->vma_end, [](atomic<pme_t> *p) {
      for (;;) {
        pme_t v = p->load();
        if (v & PTE_LOCK)
          continue;
        if (cmpxch(p, v, (pme_t) 0))
          break;
      }
    });

  return 0;
}

int
vmap::pagefault(uptr va, u32 err)
{
  atomic<pme_t> *pte = walkpgdir(pml4, va, 1);

 retry:
  pme_t ptev = pte->load();

  // optimize checks of args to syscals
  if ((ptev & (PTE_P|PTE_U|PTE_W)) == (PTE_P|PTE_U|PTE_W)) {
    // XXX using pagefault() as a security check in syscalls is prone to races
    return 0;
  }

  if (ptev & PTE_LOCK) {
    // locked, might as well wait for the other pagefaulting core..
    goto retry;
  }

  scoped_gc_epoch gc;
  vma *m = lookup(va, 1);
  if (m == 0)
    return -1;

  scoped_acquire mlock(&m->lock);
  u64 npg = (PGROUNDDOWN(va) - m->vma_start) / PGSIZE;

  if (m->n && m->n->type == ONDEMAND && m->n->page[npg] == 0)
    m = pagefault_ondemand(va, m, &mlock);

  if (vm_debug)
    cprintf("pagefault: err 0x%x va 0x%lx type %d ref %lu pid %d\n",
            err, va, m->va_type, m->n->ref.load(), myproc()->pid);

  if (m->va_type == COW && (err & FEC_WR)) {
    if (pagefault_wcow(m) < 0)
      return -1;

    mlock.release();
    tlbflush();
    goto retry;
  }

  if (!cmpxch(pte, ptev, ptev | PTE_LOCK))
    goto retry;

  if (m->deleted()) {
    *pte = ptev;
    goto retry;
  }

  if (m->va_type == COW) {
    *pte = v2p(m->n->page[npg]) | PTE_P | PTE_U | PTE_COW;
  } else {
    assert(m->n->ref == 1);
    *pte = v2p(m->n->page[npg]) | PTE_P | PTE_U | PTE_W;
  }

  return 1;
}

int
pagefault(struct vmap *vmap, uptr va, u32 err)
{
  return vmap->pagefault(va, err);
}

// Copy len bytes from p to user address va in vmap.
// Most useful when vmap is not the current page table.
int
vmap::copyout(uptr va, void *p, u64 len)
{
  char *buf = (char*)p;
  while(len > 0){
    uptr va0 = (uptr)PGROUNDDOWN(va);
    scoped_gc_epoch gc;
    vma *vma = lookup(va, 1);
    if(vma == 0)
      return -1;

    acquire(&vma->lock);
    uptr pn = (va0 - vma->vma_start) / PGSIZE;
    char *p0 = vma->n->page[pn];
    if(p0 == 0)
      panic("copyout: missing page");
    uptr n = PGSIZE - (va - va0);
    if(n > len)
      n = len;
    memmove(p0 + (va - va0), buf, n);
    len -= n;
    buf += n;
    va = va0 + PGSIZE;
    release(&vma->lock);
  }
  return 0;
}
