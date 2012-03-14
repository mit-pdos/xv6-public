#include "types.h"
#include "stat.h"
#include "user.h"
#include "uspinlock.h"

// Memory allocator by Kernighan and Ritchie,
// The C programming Language, 2nd ed.  Section 8.7.

typedef long Align;

union header {
  struct {
    union header *ptr;
    u32 size;
  } s;
  Align x;
};

typedef union header Header;

static Header base;
static Header *freep;

static struct uspinlock lock;

static void
__free(void *ap)
{
  Header *bp, *p;

  bp = (Header*)ap - 1;
  for(p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
    if(p >= p->s.ptr && (bp > p || bp < p->s.ptr))
      break;
  if(bp + bp->s.size == p->s.ptr){
    bp->s.size += p->s.ptr->s.size;
    bp->s.ptr = p->s.ptr->s.ptr;
  } else
    bp->s.ptr = p->s.ptr;
  if(p + p->s.size == bp){
    p->s.size += bp->s.size;
    p->s.ptr = bp->s.ptr;
  } else
    p->s.ptr = bp;
  freep = p;
}

void
free(void *ap)
{
  acquire(&lock);
  __free(ap);
  release(&lock);
}

static Header*
morecore(u32 nu)
{
  char *p;
  Header *hp;

  if(nu < 4096)
    nu = 4096;
  p = sbrk(nu * sizeof(Header));
  if(p == (char*)-1)
    return 0;
  hp = (Header*)p;
  hp->s.size = nu;
  __free((void*)(hp + 1));
  return freep;
}

void*
malloc(u32 nbytes)
{
  Header *p, *prevp;
  u32 nunits;

  acquire(&lock);
  nunits = (nbytes + sizeof(Header) - 1)/sizeof(Header) + 1;
  if((prevp = freep) == 0){
    base.s.ptr = freep = prevp = &base;
    base.s.size = 0;
  }
  for(p = prevp->s.ptr; ; prevp = p, p = p->s.ptr){
    if(p->s.size >= nunits){
      if(p->s.size == nunits)
        prevp->s.ptr = p->s.ptr;
      else {
        p->s.size -= nunits;
        p += p->s.size;
        p->s.size = nunits;
      }
      freep = prevp;
      release(&lock);
      return (void*)(p + 1);
    }
    if(p == freep)
      if((p = morecore(nunits)) == 0) {
        release(&lock);
        return 0;
      }
  }
}

extern "C" void initmalloc(void);
void
initmalloc(void)
{
  initlock(&lock);
}
