extern "C" {
#include "types.h"
#include "amd64.h"
#include "mmu.h"
#include "cpu.h"
#include "kernel.h"
#include "bits.h"
#include "spinlock.h"
#include "kalloc.h"
#include "queue.h"
#include "condvar.h"
#include "proc.h"
}

#include "vm.hh"
#include "gc.hh"
#include "crange.hh"
#include "cpputil.hh"

enum { vm_debug = 0 };

void
vmnode::decref()
{
  if(--ref == 0)
    delete this;
}

vma::~vma()
{
  if(n)
    n->decref();
  destroylock(&lock);
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

static struct vmnode *
vmn_copy(struct vmnode *n)
{
  struct vmnode *c = vmn_alloc(n->npages, n->type);
  if(c != 0) {
    c->type = n->type;
    if (n->type == ONDEMAND) {
      c->ip = idup(n->ip);
      c->offset = n->offset;
      c->sz = c->sz;
    } 
    if (n->page[0]) {   // If the first page is present, all of them are present
      if (c->allocpg() < 0) {
	cprintf("vmn_copy: out of memory\n");
	delete c;
	return 0;
      }
      for(u64 i = 0; i < n->npages; i++) {
	memmove(c->page[i], n->page[i], PGSIZE);
      }
    }
  }
  return c;
}

struct vmnode *
vmn_allocpg(u64 npg)
{
  struct vmnode *n = vmn_alloc(npg, EAGER);
  if (n == 0) return 0;
  if (n->allocpg() < 0) {
    delete n;
    return 0;
  }
  return n;
}

void
vmap::decref()
{
  if (--ref == 0)
    delete this;
}

vmap::vmap()
  : cr(10)
{
  snprintf(lockname, sizeof(lockname), "vmap:%p", this);
  initlock(&lock, lockname, LOCKSTAT_VM);

  ref = 1;
  alloc = 0;
  kshared = 0;

  pml4 = setupkvm();
  if (pml4 == 0) {
    cprintf("vmap_alloc: setupkvm out of memory\n");
    goto err0;
  }

  kshared = (char*) ksalloc(slab_kshared);
  if (kshared == NULL) {
    cprintf("vmap::vmap: kshared out of memory\n");
    goto err1;
  }

  if (setupkshared(pml4, kshared)) {
    cprintf("vmap::vmap: setupkshared out of memory\n");
    goto err2;
  }

  return;

 err2:
  ksfree(slab_kshared, kshared);
 err1:
  freevm(pml4);
 err0:
  destroylock(&lock);
}

vmap*
vmap_alloc()
{
  return new vmap();
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

static struct vma *
pagefault_ondemand(struct vmap *vmap, uptr va, u32 err, struct vma *m)
{
  if (m->n->allocpg() < 0)
    panic("pagefault: couldn't allocate pages");
  release(&m->lock);
  if (m->n->demand_load() < 0)
    panic("pagefault: couldn't load");
  m = vmap->lookup(va, 1);
  if (!m)
    panic("pagefault_ondemand");
  acquire(&m->lock); // re-acquire lock on m
  return m;
}

static int
pagefault_wcow(struct vmap *vmap, uptr va, pme_t *pte, struct vma *m, u64 npg)
{
  // Always make a copy of n, even if this process has the only ref, 
  // because other processes may change ref count while this process 
  // is handling wcow.
  struct vmnode *n = m->n;
  struct vmnode *c = vmn_copy(m->n);
  if (c == 0) {
    cprintf("pagefault_wcow: out of mem\n");
    return -1;
  }
  c->ref = 1;
  m->va_type = PRIVATE;
  m->n = c;
  // Update the hardware page tables to reflect the change to the vma
  updatepages(vmap->pml4, (void *) m->va_start, (void *) m->va_end, 0);
  pte = walkpgdir(vmap->pml4, (const void *)va, 0);
  *pte = v2p(m->n->page[npg]) | PTE_P | PTE_U | PTE_W;
  // drop my ref to vmnode
  n->decref();
  return 0;
}

int
pagefault(struct vmap *vmap, uptr va, u32 err)
{
  pme_t *pte = walkpgdir(vmap->pml4, (const void *)va, 1);

  // optimize checks of args to syscals
  if((*pte & (PTE_P|PTE_U|PTE_W)) == (PTE_P|PTE_U|PTE_W))
    return 0;

  gc_begin_epoch();
  vma *m = vmap->lookup(va, 1);
  if (m == 0) {
    gc_end_epoch();
    return -1;
  }

  acquire(&m->lock);
  u64 npg = (PGROUNDDOWN(va) - m->va_start) / PGSIZE;

  if (m->n && m->n->type == ONDEMAND && m->n->page[npg] == 0)
    m = pagefault_ondemand(vmap, va, err, m);

  if (vm_debug)
    cprintf("pagefault: err 0x%x va 0x%lx type %d ref %lu pid %d\n",
            err, va, m->va_type, m->n->ref.load(), myproc()->pid);

  if (m->va_type == COW && (err & FEC_WR)) {
    if (pagefault_wcow(vmap, va, pte, m, npg) < 0) {
      release(&m->lock);
      gc_end_epoch();
      return -1;
    }
  } else if (m->va_type == COW) {
    *pte = v2p(m->n->page[npg]) | PTE_P | PTE_U | PTE_COW;
  } else {
    if (m->n->ref != 1)
      panic("pagefault");
    *pte = v2p(m->n->page[npg]) | PTE_P | PTE_U | PTE_W;
  }

  // XXX(sbw) Why reload hardware page tables?
  lcr3(v2p(vmap->pml4));  // Reload hardware page tables
  release(&m->lock);
  gc_end_epoch();

  return 1;
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

// Copy len bytes from p to user address va in vmap.
// Most useful when vmap is not the current page table.
int
copyout(struct vmap *vmap, uptr va, void *p, u64 len)
{
  char *buf = (char*)p;
  while(len > 0){
    uptr va0 = (uptr)PGROUNDDOWN(va);
    gc_begin_epoch();
    vma *vma = vmap->lookup(va, 1);
    if(vma == 0) {
      gc_end_epoch();
      return -1;
    }

    acquire(&vma->lock);
    uptr pn = (va0 - vma->va_start) / PGSIZE;
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
    gc_end_epoch();
  }
  return 0;
}

struct vmnode *
vmn_alloc(u64 npg, enum vmntype type)
{
  return new vmnode(npg, type);
}

vmnode::vmnode(u64 npg, vmntype ntype)
  : npages(npg), ref(0), type(ntype), ip(0), offset(0), sz(0)
{
  if (npg > NELEM(page))
    panic("vmnode too big\n");
  memset(page, 0, sizeof(page));
}

vmap::~vmap()
{
  for (range *r: cr) {
    delete (vma*) r->value;
    cr.del(r->key, r->size);
  }

  if (kshared)
    ksfree(slab_kshared, kshared);
  if (pml4)
    freevm(pml4);
  alloc = 0;
  destroylock(&lock);
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
  if(start + len < start)
    panic("vmap::lookup bad len");

  range *r = cr.search(start, len);
  if (r != 0) {
    vma *e = (struct vma *) (r->value);
    if (e->va_end <= e->va_start)
      panic("malformed va");
    if (e->va_start < start+len && e->va_end > start)
      return e;
  }

  return 0;
}

int
vmap::insert(vmnode *n, uptr va_start)
{
  scoped_acquire sa(&lock);
  u64 len = n->npages * PGSIZE;

  if (lookup(va_start, len)) {
    cprintf("vmap_insert: overlap\n");
    return -1;
  }

  vma *e = new vma();
  if (e == 0)
    return -1;

  e->va_start = va_start;
  e->va_end = va_start + len;
  e->n = n;
  n->ref++;
  cr.add(e->va_start, len, (void *) e);
  return 0;
}

vmap*
vmap::copy(int share)
{
  struct vmap *nm = vmap_alloc();
  if(nm == 0)
    return 0;

  scoped_acquire sa(&lock);
  for (range *r: cr) {
    struct vma *e = (struct vma *) r->value;
    struct vma *ne = new vma();
    if (ne == 0)
      goto err;

    ne->va_start = e->va_start;
    ne->va_end = e->va_end;
    if (share) {
      ne->n = e->n;
      ne->va_type = COW;

      scoped_acquire sae(&e->lock);
      e->va_type = COW;
      updatepages(pml4, (void *) (e->va_start), (void *) (e->va_end), PTE_COW);
    } else {
      ne->n = vmn_copy(e->n);
      ne->va_type = e->va_type;
    }

    if (ne->n == 0)
      goto err;

    ne->n->ref++;
    nm->cr.add(ne->va_start, ne->va_end - ne->va_start, (void *) ne);
  }

  if (share)
    lcr3(v2p(pml4));  // Reload hardware page table

  return nm;

 err:
  delete nm;
  return 0;
}

int
vmap::remove(uptr va_start, uptr len)
{
  scoped_acquire sa(&lock);
  uptr va_end = va_start + len;
  struct range *r = cr.search(va_start, len);
  if (r == 0)
    panic("no vma?");
  struct vma *e = (struct vma *) r->value;
  if(e->va_start != va_start || e->va_end != va_end) {
    cprintf("vmap_remove: partial unmap unsupported\n");
    return -1;
  }
  cr.del(va_start, len);
  gc_delayed(e);
  return 0;
}
