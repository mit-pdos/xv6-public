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

static void vmap_free(void *p);

enum { vm_debug = 0 };

static void
vmn_decref(struct vmnode *n)
{
  if(--n->ref == 0)
    vmn_free(n);
}

vma::~vma()
{
  if(n)
    vmn_decref(n);
  destroylock(&lock);
}

static int
vmn_doallocpg(struct vmnode *n)
{
  for(u64 i = 0; i < n->npages; i++) {
    if((n->page[i] = kalloc()) == 0)
      return -1;
    memset((char *) n->page[i], 0, PGSIZE);
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
      if (vmn_doallocpg(c) < 0) {
	cprintf("vmn_copy: out of memory\n");
	vmn_free(c);
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
  if (vmn_doallocpg(n) < 0) {
    vmn_free(n);
    return 0;
  }
  return n;
}

void
vmap_decref(struct vmap *m)
{
  if(--m->ref == 0)
    vmap_free(m);
}

struct vmap *
vmap_alloc(void)
{
  struct vmap *m = (struct vmap *) kmalloc(sizeof(struct vmap));
  if (m == 0)
    return 0;
  memset(m, 0, sizeof(struct vmap));
  snprintf(m->lockname, sizeof(m->lockname), "vmap:%p", m);
  initlock(&m->lock, m->lockname, LOCKSTAT_VM);
  m->ref = 1;
  m->pml4 = setupkvm();
  if (m->pml4 == 0) {
    cprintf("vmap_alloc: setupkvm out of memory\n");
    destroylock(&m->lock);
    kmfree(m);
    return 0;
  }
  m->kshared = (char*)ksalloc(slab_kshared);
  if (m->kshared == NULL || setupkshared(m->pml4, m->kshared)) {
      cprintf("vmap_alloc: kshared out of memory\n");
      freevm(m->pml4);
      destroylock(&m->lock);
      kmfree(m);
      return 0;
  }
#ifdef TREE
  m->cr = new crange(10);
  if (m->cr == 0)
    return 0;
#endif
  return m;
}

static int
vmn_doload(struct vmnode *vmn, struct inode *ip, u64 offset, u64 sz)
{
  for(u64 i = 0; i < sz; i += PGSIZE){
    char *p = vmn->page[i / PGSIZE];
    s64 n;
    if(sz - i < PGSIZE)
      n = sz - i;
    else
      n = PGSIZE;
    if(readi(ip, p, offset+i, n) != n)
      return -1;
  }
  return 0;
}

// Load a program segment into a vmnode.
int
vmn_load(struct vmnode *vmn, struct inode *ip, u64 offset, u64 sz)
{
  if (vmn->type == ONDEMAND) {
    vmn->ip = ip;
    vmn->offset = offset;
    vmn->sz = sz;
    return 0;
  } else {
    return vmn_doload(vmn, ip, offset, sz);
  }
}

static struct vma *
pagefault_ondemand(struct vmap *vmap, uptr va, u32 err, struct vma *m)
{
  if (vmn_doallocpg(m->n) < 0) {
    panic("pagefault: couldn't allocate pages");
  }
  release(&m->lock);
  if (vmn_doload(m->n, m->n->ip, m->n->offset, m->n->sz) < 0) {
    panic("pagefault: couldn't load");
  }
  m = vmap_lookup(vmap, va, 1);
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
  vmn_decref(n);
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
  struct vma *m = vmap_lookup(vmap, va, 1);
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

void
vmn_free(struct vmnode *n)
{
  for(u64 i = 0; i < n->npages; i++) {
    if (n->page[i]) {
      kfree(n->page[i]);
      n->page[i] = 0;
    }
  }
  if (n->ip)
    iput(n->ip);
  n->ip = 0;
  kmfree(n);
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
    struct vma *vma = vmap_lookup(vmap, va, 1);
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
  struct vmnode *n = (struct vmnode *) kmalloc(sizeof(struct vmnode));
  if (n == 0) {
    cprintf("out of vmnodes");
    return 0;
  }
  if(npg > NELEM(n->page)) {
    panic("vmnode too big\n");
  }
  memset(n, 0, sizeof(struct vmnode));
  n->npages = npg;
  n->type = type;
  return n;
}

#ifdef TREE
static void
vmap_free(void *p)
{
  struct vmap *m = (struct vmap *) p;

  for (range *r: m->cr) {
    delete (vma*) r->value;
    r->cr->del(r->key, r->size);
  }

  delete m->cr;
  ksfree(slab_kshared, m->kshared);
  freevm(m->pml4);
  m->pml4 = 0;
  m->alloc = 0;
  destroylock(&m->lock);
  kmfree(m);
}

// Does any vma overlap start..start+len?
// If yes, return the vma pointer.
// If no, return 0.
// This code can't handle regions at the very end
// of the address space, e.g. 0xffffffff..0x0
// We key vma's by their end address.
struct vma *
vmap_lookup(struct vmap *m, uptr start, uptr len)
{
  if(start + len < start)
    panic("vmap_lookup bad len");

  range *r = m->cr->search(start, len);
  if (r != 0) {
    struct vma *e = (struct vma *) (r->value);
    if (e->va_end <= e->va_start) 
      panic("malformed va");
    if (e->va_start < start+len && e->va_end > start) {
        return e;
    }
  }

  return 0;
}

int
vmap_insert(struct vmap *m, struct vmnode *n, uptr va_start)
{
  acquire(&m->lock);
  u64 len = n->npages * PGSIZE;

  if(vmap_lookup(m, va_start, len)){
    cprintf("vmap_insert: overlap\n");
    release(&m->lock);
    return -1;
  }

  struct vma *e = new vma();
  if (e == 0) {
    release(&m->lock);
    return -1;
  }
  e->va_start = va_start;
  e->va_end = va_start + len;
  e->n = n;
  n->ref++;
  m->cr->add(e->va_start, len, (void *) e);
  release(&m->lock);
  return 0;
}

struct vmap *
vmap_copy(struct vmap *m, int share)
{
  struct vmap *nm = vmap_alloc();
  if(nm == 0)
    return 0;

  acquire(&m->lock);
  for (range *r: m->cr) {
    struct vma *e = (struct vma *) r->value;
    struct vma *ne = new vma();
    if (ne == 0)
      goto err;

    ne->va_start = e->va_start;
    ne->va_end = e->va_end;
    if (share) {
      ne->n = e->n;
      ne->va_type = COW;
      acquire(&e->lock);
      e->va_type = COW;
      updatepages(m->pml4, (void *) (e->va_start), (void *) (e->va_end), PTE_COW);
      release(&e->lock);
    } else {
      ne->n = vmn_copy(e->n);
      ne->va_type = e->va_type;
    }

    if(ne->n == 0)
      goto err;

    ne->n->ref++;
    nm->cr->add(ne->va_start, ne->va_end - ne->va_start, (void *) ne);
  }

  if (share)
    lcr3(v2p(m->pml4));  // Reload hardware page table

  release(&m->lock);
  return nm;

 err:
  vmap_free(nm);
  release(&m->lock);
  return 0;
}

int
vmap_remove(struct vmap *m, uptr va_start, u64 len)
{
  acquire(&m->lock);
  uptr va_end = va_start + len;
  struct range *r = m->cr->search(va_start, len);
  if (r == 0)
    panic("no vma?");
  struct vma *e = (struct vma *) r->value;
  if(e->va_start != va_start || e->va_end != va_end) {
    cprintf("vmap_remove: partial unmap unsupported\n");
    release(&m->lock);
    return -1;
  }
  m->cr->del(va_start, len);
  gc_delayed(e);
  release(&m->lock);
  return 0;
}

#else // !TREE

static void
vmap_free(void *p)
{
  struct vmap *m = (struct vmap *) p;
  for(u64 i = 0; i < NELEM(m->e); i++) {
    if (m->e[i])
      delete m->e[i];
  }
  freevm(m->pml4);
  m->pml4 = 0;
  m->alloc = 0;
}

// Does any vma overlap start..start+len?
// If yes, return the vma pointer.
// If no, return 0.
// This code can't handle regions at the very end
// of the address space, e.g. 0xffffffff..0x0
// We key vma's by their end address.
struct vma *
vmap_lookup(struct vmap *m, uptr start, uptr len)
{
  if(start + len < start)
    panic("vmap_lookup bad len");

  for(u64 i = 0; i < NELEM(m->e); i++){
    struct vma *e = m->e[i];
    if(e) {
      if(e->va_end <= e->va_start)  // XXX shouldn't this involve start and len?
        panic("vmap_lookup bad vma");
      if(e->va_start < start+len && e->va_end > start)
        return e;
    }
  }
  return 0;
}

int
vmap_insert(struct vmap *m, struct vmnode *n, uptr va_start)
{
  acquire(&m->lock);
  u64 len = n->npages * PGSIZE;

  if(vmap_lookup(m, va_start, len)){
    cprintf("vmap_insert: overlap\n");
    release(&m->lock);
    return -1;
  }

  for(u64 i = 0; i < NELEM(m->e); i++) {
    if(m->e[i])
      continue;
    m->e[i] = new vma();
    if (m->e[i] == 0)
      return -1;
    m->e[i]->va_start = va_start;
    m->e[i]->va_end = va_start + len;
    m->e[i]->n = n;
    __sync_fetch_and_add(&n->ref, 1);
    release(&m->lock);
    return 0;
  }
  release(&m->lock);

  cprintf("vmap_insert: out of vma slots\n");
  return -1;
}

struct vmap *
vmap_copy(struct vmap *m, int share)
{
  struct vmap *c = vmap_alloc();
  if(c == 0)
    return 0;

  acquire(&m->lock);
  for(u32 i = 0; i < NELEM(m->e); i++) {
    if(m->e[i] == 0)
      continue;
    c->e[i] = new vma();
    if (c->e[i] == 0) {
      release(&m->lock);
      vmap_free(c);
      return 0;
    }
    c->e[i]->va_start = m->e[i]->va_start;
    c->e[i]->va_end = m->e[i]->va_end;
    if (share) {
      c->e[i]->n = m->e[i]->n;
      c->e[i]->va_type = COW;

      acquire(&m->e[i]->lock);
      m->e[i]->va_type = COW;
      updatepages(m->pml4, (void *) (m->e[i]->va_start), (void *) (m->e[i]->va_end), PTE_COW);
      release(&m->e[i]->lock);
    } else {
      c->e[i]->n = vmn_copy(m->e[i]->n);
      c->e[i]->va_type = m->e[i]->va_type;
    }
    if(c->e[i]->n == 0) {
      release(&m->lock);
      vmap_free(c);
      return 0;
    }
    __sync_fetch_and_add(&c->e[i]->n->ref, 1);
  }
  if (share)
    lcr3(v2p(m->pml4));  // Reload hardware page table

  release(&m->lock);
  return c;
}

int
vmap_remove(struct vmap *m, uptr va_start, u64 len)
{
  acquire(&m->lock);
  uptr va_end = va_start + len;
  for(u32 i = 0; i < NELEM(m->e); i++) {
    if(m->e[i] && (m->e[i]->va_start < va_end && m->e[i]->va_end > va_start)) {
      if(m->e[i]->va_start != va_start || m->e[i]->va_end != va_end) {
	release(&m->lock);
	cprintf("vmap_remove: partial unmap unsupported\n");
	return -1;
      }
      gc_delayed(m->e[i]);
      m->e[i] = 0;
    }
  }
  release(&m->lock);
  return 0;
}
#endif
