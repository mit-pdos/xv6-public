/*
 * physical memory allocator, intended to be used to allocate
 * memory for user processes. allocates in 4096-byte "pages".
 * free list is sorted and combines adjacent pages into
 * long runs, to make it easier to allocate big segments.
 * one reason the page size is 4k is that the x86 segment size
 * granularity is 4k.
 */

#include "param.h"
#include "types.h"
#include "defs.h"
#include "param.h"
#include "mmu.h"
#include "proc.h"
#include "spinlock.h"

struct spinlock kalloc_lock;

struct run {
  struct run *next;
  int len; // bytes
};
struct run *freelist;

void ktest();

/*
 * initialize free list of physical pages. this code
 * cheats by just considering the one megabyte of pages
 * after _end.
 */
void
kinit()
{
  extern int end;
  unsigned mem;
  char *start;
  
  start = (char *) &end;
  start = (char *) (((unsigned)start + PAGE) & ~(PAGE-1));
  mem = 256; // XXX
  cprintf("mem = %d\n", mem * PAGE);
  kfree(start, mem * PAGE);
  ktest();
}

void
kfree(char *cp, int len)
{
  struct run **rr;
  struct run *p = (struct run *) cp;
  struct run *pend = (struct run *) (cp + len);
  int i;

  if(len % PAGE)
    panic("kfree");

  // XXX fill with junk to help debug
  for(i = 0; i < len; i++)
    cp[i] = 1;

  acquire(&kalloc_lock);

  rr = &freelist;
  while(*rr){
    struct run *rend = (struct run *) ((char *)(*rr) + (*rr)->len);
    if(p >= *rr && p < rend)
      panic("freeing free page");
    if(pend == *rr){
      p->len = len + (*rr)->len;
      p->next = (*rr)->next;
      *rr = p;
      goto out;
    }
    if(pend < *rr){
      p->len = len;
      p->next = *rr;
      *rr = p;
      goto out;
    }
    if(p == rend){
      (*rr)->len += len;
      if((*rr)->next && (*rr)->next == pend){
        (*rr)->len += (*rr)->next->len;
        (*rr)->next = (*rr)->next->next;
      }
      goto out;
    }
    rr = &((*rr)->next);
  }
  p->len = len;
  p->next = 0;
  *rr = p;

 out:
  release(&kalloc_lock);
}

/*
 * allocate n bytes of physical memory.
 * returns a kernel-segment pointer.
 * returns 0 if there's no run that's big enough.
 */
char *
kalloc(int n)
{
  struct run **rr;

  if(n % PAGE)
    panic("kalloc");

  acquire(&kalloc_lock);

  rr = &freelist;
  while(*rr){
    struct run *r = *rr;
    if(r->len == n){
      *rr = r->next;
      release(&kalloc_lock);
      return (char *) r;
    }
    if(r->len > n){
      char *p = (char *)r + (r->len - n);
      r->len -= n;
      release(&kalloc_lock);
      return p;
    }
    rr = &(*rr)->next;
  }
  release(&kalloc_lock);
  return 0;
}

void
ktest()
{
  char *p1, *p2, *p3;

  // test coalescing
  p1 = kalloc(4 * PAGE);
  kfree(p1 + 3*PAGE, PAGE);
  kfree(p1 + 2*PAGE, PAGE);
  kfree(p1, PAGE);
  kfree(p1 + PAGE, PAGE);
  p2 = kalloc(4 * PAGE);
  if(p2 != p1)
    panic("ktest");
  kfree(p2, 4 * PAGE);

  // test finding first run that fits
  p1 = kalloc(1 * PAGE);
  p2 = kalloc(1 * PAGE);
  kfree(p1, PAGE);
  p3 = kalloc(2 * PAGE);
  kfree(p2, PAGE);
  kfree(p3, 2 * PAGE);

  // test running out of memory
  p1 = 0;
  while(1){
    p2 = kalloc(PAGE);
    if(p2 == 0)
      break;
    *(char **)p2 = p1;
    p1 = p2;
  }
  while(p1){
    p2 = *(char **)p1;
    kfree(p1, PAGE);
    p1 = p2;
  }
  p1 = kalloc(PAGE * 20);
  if(p1 == 0)
    panic("ktest2");
  kfree(p1, PAGE * 20);
}
