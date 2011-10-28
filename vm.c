#include "param.h"
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
#include <stddef.h>

// Set up CPU's kernel segment descriptors.
// Run once at boot time on each CPU.
void
inittls(void)
{
  struct cpu *c;

  // Initialize cpu-local storage.
  c = &cpus[cpunum()];
  writegs(KDSEG);
  writemsr(MSR_GS_BASE, (u64)&c->cpu);
  c->cpu = c;
  c->proc = NULL;
  c->kmem = &kmems[cpunum()];
}

static struct vma *
vma_alloc(void)
{
  struct vma *e = kmalloc(sizeof(struct vma));
  if (e == 0)
    return 0;
  memset(e, 0, sizeof(struct vma));
  e->va_type = PRIVATE;
  snprintf(e->lockname, sizeof(e->lockname), "vma:%p", e);
  initlock(&e->lock, e->lockname);
  return e;
}

static void
vmn_decref(struct vmnode *n)
{
  if(subfetch(&n->ref, 1) == 0)
    vmn_free(n);
}

static void
vma_free(void *p)
{
  struct vma *e = (struct vma *) p;
  if(e->n)
    vmn_decref(e->n);
  kmfree(e);
}

#ifdef TREE
struct state {
  int share;
  void *pgdir;
  struct node *root;
};

static int
vmap_free_vma(struct kv *kv, void *p)
{  
  struct state *st = (struct state *) p;
  vma_free(kv->val);
  st->root = tree_remove(st->root, kv->key);
  return 1;
}

static void
vmap_free(void *p)
{
  struct vmap *m = (struct vmap *) p;
  struct state *st = kmalloc(sizeof(struct state));
  st->root = m->root;
  tree_foreach(m->root, vmap_free_vma, st);
  m->root = st->root;
  freevm(m->pgdir);
  kmfree(st);
  m->pgdir = 0;
  m->alloc = 0;
}
#else
static void
vmap_free(void *p)
{
  struct vmap *m = (struct vmap *) p;
  for(u64 i = 0; i < NELEM(m->e); i++) {
    if (m->e[i])
      vma_free(m->e[i]);
  }
  freevm(m->pml4);
  m->pml4 = 0;
  m->alloc = 0;
}
#endif

void
vmap_decref(struct vmap *m)
{
  if(subfetch(&m->ref, 1) == 0)
    vmap_free(m);
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

#ifdef TREE
  struct kv *kv = tree_find_gt(m->root, start);   // find vma with va_end > start
  if (kv != 0) {
    struct vma *e = (struct vma *) (kv->val);
    if (e->va_end <= e->va_start) 
      panic("malformed va");
    if (e->va_start < start+len && e->va_end > start) {
        return e;
    }
  }
#else
  for(u64 i = 0; i < NELEM(m->e); i++){
    struct vma *e = m->e[i];
    if(e) {
      if(e->va_end <= e->va_start)  // XXX shouldn't this involve start and len?
        panic("vmap_lookup bad vma");
      if(e->va_start < start+len && e->va_end > start)
        return e;
    }
  }
#endif
  return 0;
}

struct vmap *
vmap_alloc(void)
{
  struct vmap *m = kmalloc(sizeof(struct vmap));
  if (m == 0)
    return 0;

  memset(m, 0, sizeof(struct vmap));
  snprintf(m->lockname, sizeof(m->lockname), "vmap:%p", m);
  initlock(&m->lock, m->lockname);
  m->ref = 1;
  m->pml4 = setupkvm();
  if (m->pml4 == 0) {
    cprintf("vmap_alloc: setupkvm out of memory\n");
    kmfree(m);
    return 0;
  }
  return m;
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

#ifdef TREE
  struct vma *e = vma_alloc();
  struct kv kv;
  if (e == 0) {
    release(&m->lock);
    return -1;
  }
  e->va_start = va_start;
  e->va_end = va_start + len;
  e->n = n;
  __sync_fetch_and_add(&n->ref, 1);
  kv.key = e->va_end;
  kv.val = e;
  m->root = tree_insert(m->root, &kv);
  release(&m->lock);
  return 0;
#else 
  for(u64 i = 0; i < NELEM(m->e); i++) {
    if(m->e[i])
      continue;
    m->e[i] = vma_alloc();
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
#endif
}

struct vmnode *
vmn_alloc(u64 npg, enum vmntype type)
{
  struct vmnode *n = kmalloc(sizeof(struct vmnode));
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
    rcu_begin_read();
    struct vma *vma = vmap_lookup(vmap, va, 1);
    if(vma == 0) {
      rcu_end_read();
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
    rcu_end_read();
  }
  return 0;
}

struct vmnode *
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

static int
vmn_doload(struct vmnode *vmn, struct inode *ip, u64 offset, u64 sz)
{
  for(u64 i = 0; i < sz; i += PGSIZE){
    char *p = vmn->page[i / PGSIZE];
    u64 n;
    if(sz - i < PGSIZE)
      n = sz - i;
    else
      n = PGSIZE;
    if(readi(ip, p, offset+i, n) != n)
      return -1;
  }
  return 0;
}

struct vmap *
vmap_copy(struct vmap *m, int share)
{
  struct vmap *c = vmap_alloc();
  if(c == 0)
    return 0;

  acquire(&m->lock);
#ifdef TREE
  struct state *st = kmalloc(sizeof(struct state));
  st->share = share;
  st->pgdir = m->pgdir;
  st->root = c->root;
  if (!tree_foreach(m->root, vmap_copy_vma, st)) {
    vmap_free(c);
    release(&m->lock);
    kmfree(st);
    return 0;
  }
  c->root = st->root;
  kmfree(st);
#else
  for(int i = 0; i < NELEM(m->e); i++) {
    if(m->e[i] == 0)
      continue;
    c->e[i] = vma_alloc();
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
#endif
  if (share)
    lcr3(v2p(m->pml4));  // Reload hardware page table

  release(&m->lock);
  return c;
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

  rcu_begin_read();
  struct vma *m = vmap_lookup(vmap, va, 1);
  if (m == 0) {
    rcu_end_read();
    return -1;
  }

  acquire(&m->lock);
  u64 npg = (PGROUNDDOWN(va) - m->va_start) / PGSIZE;

  if (m->n && m->n->type == ONDEMAND && m->n->page[npg] == 0)
    m = pagefault_ondemand(vmap, va, err, m);

  if (m->va_type == COW && (err & FEC_WR)) {
    if (pagefault_wcow(vmap, va, pte, m, npg) < 0) {
      release(&m->lock);
      rcu_end_read();
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
  rcu_end_read();

  return 1;
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

int
vmap_remove(struct vmap *m, uptr va_start, u64 len)
{
  acquire(&m->lock);
  uptr va_end = va_start + len;
#ifdef TREE
  struct kv *kv = tree_find_gt(m->root, va_start);
  if (kv == 0)
    panic("no vma?");
  struct vma *e = (struct vma *) kv->val;
  if(e->va_start != va_start || e->va_end != va_end) {
    cprintf("vmap_remove: partial unmap unsupported\n");
    release(&m->lock);
    return -1;
  }
  m->root = tree_remove(m->root, va_start+len);
  rcu_delayed(e, vma_free);
#else
  for(int i = 0; i < NELEM(m->e); i++) {
    if(m->e[i] && (m->e[i]->va_start < va_end && m->e[i]->va_end > va_start)) {
      if(m->e[i]->va_start != va_start || m->e[i]->va_end != va_end) {
	release(&m->lock);
	cprintf("vmap_remove: partial unmap unsupported\n");
	return -1;
      }
      rcu_delayed(m->e[i], vma_free);
      m->e[i] = 0;
    }
  }
#endif
  release(&m->lock);
  return 0;
}
