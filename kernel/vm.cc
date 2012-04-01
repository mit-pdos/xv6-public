#include "types.h"
#include "amd64.h"
#include "mmu.h"
#include "cpu.hh"
#include "kernel.hh"
#include "bits.hh"
#include "spinlock.h"
#include "kalloc.hh"
#include "queue.h"
#include "condvar.h"
#include "proc.hh"
#include "vm.hh"
#include "gc.hh"
#include "crange.hh"
#include "cpputil.hh"
#include "sperf.hh"
#include "uwq.hh"
#include "kmtrace.hh"

enum { vm_debug = 0 };
enum { tlb_shootdown = 1 };
enum { tlb_lazy = 1 };

/*
 * vmnode
 */

vmnode::vmnode(u64 npg, vmntype ntype, inode *i, u64 off, u64 s)
  : npages(npg), type(ntype), ip(i), offset(off), sz(s), ref_(0)
{
  if (npg > NELEM(page))
    panic("vmnode too big\n");
  memset(page, 0, npg * sizeof(page[0]));
  if (type == EAGER && ip) {
    assert(allocpg(false) == 0);
    assert(demand_load() == 0);
  }
}

vmnode::~vmnode()
{
  for(u64 i = 0; i < npages; i++)
    if (page[i])
      kfree(page[i]);
  if (ip)
    iput(ip);
}

void
vmnode::decref(void)
{
  if(--ref_ == 0)
    delete this;
}

void
vmnode::incref(void)
{
  ++ref_;
}

u64
vmnode::ref(void)
{
  return ref_.load();
}

int
vmnode::allocpg(bool zero)
{
  for(u64 i = 0; i < npages; i++) {
    if (page[i])
      continue;

    char *p;
    if (zero)
      p = zalloc("(vmnode::allocpg)");
    else
      p = kalloc("(vmnode::allocpg)");

    if (!p) {
      cprintf("allocpg: OOM -- leaving half-filled vmnode\n");
      return -1;
    }

    if(!cmpxch(&page[i], (char*) 0, p)) {
      if (zero)
        zfree(p);
      else
        kfree(p);
    }
  }
  return 0;
}

vmnode *
vmnode::copy()
{
  vmnode *c = new vmnode(npages, type,
                         (type==ONDEMAND) ? idup(ip) : 0,
                         offset, sz);
  if(c == 0)
    return 0;

  if (!page[0])   // If first page is absent, all pages are absent
    return c;

  if (c->allocpg(false) < 0) {
    cprintf("vmn_copy: out of memory\n");
    delete c;
    return 0;
  }
  for(u64 i = 0; i < npages; i++)
    if (page[i])
      memmove(c->page[i], page[i], PGSIZE);

  return c;
}

int
vmnode::demand_load()
{
#ifdef MTRACE 
  mtreadavar("inode:%x.%x", ip->dev, ip->inum);
  mtwriteavar("vmnode:%016x", this);
#endif
  for (u64 i = 0; i < sz; i += PGSIZE) {
    char *p = page[i / PGSIZE];
    s64 n;
    if (sz - i < PGSIZE)
      n = sz - i;
    else
      n = PGSIZE;

    /*
     * Possible race condition with concurrent demand_load() calls,
     * if the underlying inode's contents change..
     */
    if (readi(ip, p, offset+i, n) != n)
      return -1;
  }
  return 0;
}

/*
 * vma
 */

vma::vma(vmap *vmap, uptr start, uptr end, enum vmatype vtype, vmnode *vmn) :
#if VM_CRANGE
    range(&vmap->cr, start, end-start),
#endif
    vma_start(start), vma_end(end), va_type(vtype), n(vmn)
{
  if (n)
    n->incref();
}

vma::~vma()
{
  if (n)
    n->decref();
}

/*
 * vmap
 */

vmap*
vmap::alloc(void)
{
  return new vmap();
}

vmap::vmap() : 
#if VM_CRANGE
  cr(10),
#endif
#if VM_RADIX
  rx(PGSHIFT),
#endif
  ref(1), pml4(setupkvm()), kshared((char*) ksalloc(slab_kshared)),
  brk_(0)
{
  initlock(&brklock_, "brk_lock", LOCKSTAT_VM);
  if (pml4 == 0) {
    cprintf("vmap_alloc: setupkvm out of memory\n");
    goto err;
  }

  if (kshared == nullptr) {
    cprintf("vmap::vmap: kshared out of memory\n");
    goto err;
  }

  if (mapkva(pml4, kshared, KSHARED, KSHAREDSIZE)) {
    cprintf("vmap::vmap: mapkva out of memory\n");
    goto err;
  }

  return;

 err:
  if (kshared)
    ksfree(slab_kshared, kshared);
  if (pml4)
    freevm(pml4);
}

vmap::~vmap()
{
  if (kshared)
    ksfree(slab_kshared, kshared);
  if (pml4)
    freevm(pml4);
  destroylock(&brklock_);
}

void
vmap::decref()
{
  if (--ref == 0)
    delete this;
}

void
vmap::incref()
{
  ++ref;
}

bool
vmap::replace_vma(vma *a, vma *b)
{
#if VM_CRANGE
  auto span = cr.search_lock(a->vma_start, a->vma_end - a->vma_start);
#endif
#if VM_RADIX
  auto span = rx.search_lock(a->vma_start, a->vma_end - a->vma_start);
#endif
  if (a->deleted())
    return false;
  for (auto e: span)
    assert(a == e);
#if VM_CRANGE
  span.replace(b);
#endif
#if VM_RADIX
  span.replace(a->vma_start, b->vma_start-a->vma_start, 0);
  span.replace(b->vma_start, b->vma_end-b->vma_start, b);
  span.replace(b->vma_end, a->vma_end-b->vma_end, 0);
#endif
  return true;
}

vmap*
vmap::copy(int share)
{
  vmap *nm = new vmap();
  if(nm == 0)
    return 0;

#if VM_CRANGE
  for (auto r: cr) {
#endif
#if VM_RADIX
  void *last = 0;
  for (auto r: rx) {
    if (!r || r == last)
      continue;
    last = r;
#endif
    vma *e = (vma *) r;

    struct vma *ne;
    if (share) {
      ne = new vma(nm, e->vma_start, e->vma_end, COW, e->n);

      // if the original vma wasn't COW, replace it with a COW vma
      if (e->va_type != COW) {
        vma *repl = new vma(this, e->vma_start, e->vma_end, COW, e->n);
        replace_vma(e, repl);
#if VM_RADIX
        last = repl;
#endif
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
      ne = new vma(nm, e->vma_start, e->vma_end, PRIVATE, e->n->copy());
    }

    if (ne == 0)
      goto err;

    if (ne->n == 0) {
      delete ne;
      goto err;
    }

#if VM_CRANGE
    auto span = nm->cr.search_lock(ne->vma_start, ne->vma_end - ne->vma_start);
#endif
#if VM_RADIX
    auto span = nm->rx.search_lock(ne->vma_start, ne->vma_end - ne->vma_start);
#endif
    for (auto x: span) {
#if VM_RADIX
      if (!x)
        continue;
#endif
      cprintf("non-empty span 0x%lx--0x%lx in %p: %p\n",
              ne->vma_start, ne->vma_end, nm, x);
      assert(0);  /* span must be empty */
    }
#if VM_CRANGE
    span.replace(ne);
#endif
#if VM_RADIX
    span.replace(ne->vma_start, ne->vma_end-ne->vma_start, ne);
#endif
  }

  if (share) {
    // Reload hardware page table
    if (tlb_shootdown) {
      tlbflush();
    } else {
      lcr3(rcr3());
    }
  }

  nm->brk_ = brk_;

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

#if VM_CRANGE
  auto r = cr.search(start, len);
#endif
#if VM_RADIX
  assert(len <= PGSIZE);
  auto r = rx.search(start);
#endif
  if (r != 0) {
    vma *e = (vma *) r;
    if (e->vma_end <= e->vma_start)
      panic("malformed va");
    if (e->vma_start < start+len && e->vma_end > start)
      return e;
  }

  return 0;
}

long
vmap::insert(vmnode *n, uptr vma_start, int dotlb)
{
  ANON_REGION("vmap::insert", &perfgroup);

  vma *e;
  bool replaced = false;
  bool fixed = (vma_start != 0);

again:
  if (!fixed) {
    vma_start = unmapped_area(n->npages);
    if (vma_start == 0) {
      cprintf("vmap::insert: no unmapped areas\n");
      return -1;
    }
  }

  {
    // new scope to release the search lock before tlbflush
    u64 len = n->npages * PGSIZE;
#if VM_CRANGE
    auto span = cr.search_lock(vma_start, len);
#endif
#if VM_RADIX
    auto span = rx.search_lock(vma_start, len);
#endif
    for (auto r: span) {
#if VM_RADIX
      if (!r)
        continue;
#endif
      if (!fixed)
        goto again;

      vma *rvma = (vma*) r;
      cprintf("vmap::insert: overlap with %p: 0x%lx--0x%lx\n",
              rvma, rvma->vma_start, rvma->vma_end);
      return -1;
    }

    // XXX handle overlaps, set replaced=true

    e = new vma(this, vma_start, vma_start+len, PRIVATE, n);
    if (e == 0) {
      cprintf("vmap::insert: out of vmas\n");
      return -1;
    }

#if VM_CRANGE
    span.replace(e);
#endif
#if VM_RADIX
    span.replace(e->vma_start, e->vma_end-e->vma_start, e);
#endif
  }

  bool needtlb = false;
  if (replaced)
    updatepages(pml4, e->vma_start, e->vma_end, [&needtlb](atomic<pme_t> *p) {
        for (;;) {
          pme_t v = p->load();
          if (v & PTE_LOCK)
            continue;
          if (!(v & PTE_P))
            break;
          if (cmpxch(p, v, (pme_t) 0)) {
            needtlb = true;
            break;
          }
        }
      });
  if (tlb_shootdown) {
    if (needtlb && dotlb)
      tlbflush();
    else
      if (tlb_lazy)
        tlbflush(myproc()->unmap_tlbreq_);
  }

  return vma_start;
}

int
vmap::remove(uptr vma_start, uptr len)
{
  {
    // new scope to release the search lock before tlbflush
    uptr vma_end = vma_start + len;

#if VM_CRANGE
    auto span = cr.search_lock(vma_start, len);
#endif
#if VM_RADIX
    auto span = rx.search_lock(vma_start, len);
#endif
    for (auto r: span) {
      vma *rvma = (vma*) r;
      if (rvma->vma_start < vma_start || rvma->vma_end > vma_end) {
        cprintf("vmap::remove: partial unmap not supported\n");
        return -1;
      }
    }

    // XXX handle partial unmap

#if VM_CRANGE
    span.replace(0);
#endif
#if VM_RADIX
    span.replace(vma_start, len, 0);
#endif
  }

  bool needtlb = false;
  updatepages(pml4, vma_start, vma_start + len, [&needtlb](atomic<pme_t> *p) {
      for (;;) {
        pme_t v = p->load();
        if (v & PTE_LOCK)
          continue;
        if (!(v & PTE_P))
          break;
        if (cmpxch(p, v, (pme_t) 0)) {
          needtlb = true;
          break;
        }
      }
    });
  if (tlb_shootdown && needtlb) {
    if (tlb_lazy) {
      myproc()->unmap_tlbreq_ = tlbflush_req++;
    } else {
      tlbflush();
    }
  }
  return 0;
}

/*
 * pagefault handling code on vmap
 */

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

  vma *repl = new vma(this, m->vma_start, m->vma_end, PRIVATE, nodecopy);

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
  if (va >= USERTOP)
    return -1;

  atomic<pme_t> *pte = walkpgdir(pml4, va, 1);
  if (pte == nullptr) {
    cprintf("pagefault: couldn't allocate page table\n");
    return -1;
  }

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

  u64 npg = (PGROUNDDOWN(va) - m->vma_start) / PGSIZE;
  if (vm_debug)
    cprintf("pagefault: err 0x%x va 0x%lx type %d ref %lu pid %d\n",
            err, va, m->va_type, m->n->ref(), myproc()->pid);

  if (m->n && !m->n->page[npg])
    if (m->n->allocpg() < 0)
      panic("pagefault: couldn't allocate pages");

  if (m->n && m->n->type == ONDEMAND)
    if (m->n->demand_load() < 0)
      panic("pagefault: couldn't load");

  if (m->va_type == COW && (err & FEC_WR)) {
    if (pagefault_wcow(m) < 0)
      return -1;

    if (tlb_shootdown) {
      tlbflush();
    } else {
      lcr3(rcr3());
    }
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
    *pte = v2p(m->n->page[npg]) | PTE_P | PTE_U | PTE_W;
  }

  mtreadavar("vmnode:%016x", m->n);

  return 1;
}

int
pagefault(struct vmap *vmap, uptr va, u32 err)
{
#if MTRACE
  mt_ascope ascope("%s(%p)", __func__, va);
  mtwriteavar("thread:%x", myproc()->pid);
  mtwriteavar("page:%p.%016x", vmap, PGROUNDDOWN(va));
#endif

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

    vma->n->allocpg();
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
  }
  return 0;
}

int
vmap::sbrk(ssize_t n, uptr *addr)
{
  scoped_acquire xlock(&brklock_);
  auto curbrk = brk_;
  *addr = curbrk;

  if(n < 0 && 0 - n <= curbrk){
    brk_ += n;
    return 0;
  }

  if(n < 0 || n > USERTOP || curbrk + n > USERTOP)
    return -1;

  // look one page ahead, to check if the newly allocated region would
  // abut the next-higher vma? we can't allow that, since then a future
  // sbrk() would start to use the next region (e.g. the stack).
  uptr newstart = PGROUNDUP(curbrk);
  s64 newn = PGROUNDUP(n + curbrk - newstart);
#if VM_CRANGE
  range *prev = 0;
  auto span = cr.search_lock(newstart, newn + PGSIZE);
#endif
#if VM_RADIX
  auto span = rx.search_lock(newstart, newn + PGSIZE);
#endif
#if VM_CRANGE
  for (auto r: span) {
#endif
#if VM_RADIX
  void *last = 0;
  for (auto r: span) {
    if (!r || r == last)
      continue;
    last = r;
#endif
    vma *e = (vma*) r;

    if (e->vma_start <= newstart) {
      if (e->vma_end >= newstart + newn) {
        brk_ += n;
        return 0;
      }

      newn -= e->vma_end - newstart;
      newstart = e->vma_end;
#if VM_CRANGE
      prev = e;
#endif
    } else {
      cprintf("growproc: overlap with existing mapping; brk %lx n %ld\n",
              curbrk, n);
      return -1;
    }
  }

  vmnode *vmn = new vmnode(newn / PGSIZE);
  if(vmn == 0){
    cprintf("growproc: vmn_allocpg failed\n");
    return -1;
  }

  vma *repl = new vma(this, newstart, newstart+newn, PRIVATE, vmn);
  if (!repl) {
    cprintf("growproc: out of vma\n");
    delete vmn;
    return -1;
  }

#if VM_CRANGE
  span.replace(prev, repl);
#endif

#if VM_RADIX
  span.replace(newstart, newn, repl);
#endif

  brk_ += n;
  return 0;
}

uptr
vmap::unmapped_area(size_t npages)
{
  size_t n = npages*PGSIZE;
  uptr addr = 0x1000;

  while (addr < USERTOP) {
#if VM_CRANGE
    auto x = cr.search(addr, n);
    if (x == nullptr)
      return addr;
    vma* a = (vma*) x;
    addr = a->vma_end;
#endif

#if VM_RADIX
    bool overlap = false;
    for (uptr ax = addr; ax < addr+n; ax += PGSIZE) {
      auto x = rx.search(ax);
      if (x != nullptr) {
        overlap = true;
        vma* a = (vma*) x;
        addr = a->vma_end;
        break;
      }
    }
    if (!overlap)
      return addr;
#endif
  }
  return 0;
}
