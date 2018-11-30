#include "types.h"
#include "stat.h"
#include "user.h"
#include "param.h"

// Memory allocator by Kernighan and Ritchie,
// The C programming Language, 2nd ed.  Section 8.7.

typedef long Align;

union header {
  struct {
    union header *ptr;
    uint size;
  } s;
  Align x;
};

typedef union header Header;

static Header base;
static Header *freep;

void
free(void *ap)
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

static Header*
morecore(uint nu)
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
  free((void*)(hp + 1));
  return freep;
}

void*
malloc(uint nbytes)
{
  Header *p, *prevp;
  uint nunits;

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
      return (void*)(p + 1);
    }
    if(p == freep)
      if((p = morecore(nunits)) == 0)
        return 0;
  }
}

void*
urealloc(void* curp, uint nbytes)
{
  void *newp;

  Header *chp = (Header) curp;
  Header *nhp = (Header) newp;

  if (!chp) {
    nhp = malloc(nbytes);
    printf(1,"not exist! new it!\t curp=%p, size=%d, nbytes=%d \n", curp, chp->s.size, nbytes);
    if (!nhp) { goto error; }
  } else {
    if (chp->s.size < nbytes) {
      printf(1,"exist! new bigger.\t curp=%p, size=%d, nbytes=%d \n", curp, chp->s.size, nbytes);
      nhp = malloc(nbytes);
      if (!nhp) { goto error; }
      memmove(nhp, chp, chp->s.size);
      free(chp);
    } else {
      printf(1,"exist! use curr.\t curp=%p, size=%d, nbytes=%d \n", curp, chp->s.size, nbytes);
      nhp = chp;
    }
  }

  printf(1,"curp: %p, newp: %p, nbytes:%d \n", curp, newp, nbytes);

  return newp;
error:
  return 0;
}

//cmorecore

static Header*
cmorecore(uint nu)
{
    char *p;
    Header *hp;
    
    if(nu < 4096)
        nu = 4096;
    p = csbrk(nu * sizeof(Header));
    printf(1,"called cmorecore\n");
    if(p == (char*)-1)
        return 0;
    hp = (Header*)p;
    hp->s.size = nu;
    free((void*)(hp + 1));
    return freep;
}

//cmalloc

void*
cmalloc(uint nbytes)
{
    Header *p, *prevp;
    uint nunits;
    
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
            return (void*)(p + 1);
        }
        if(p == freep)
            if((p = cmorecore(nunits)) == 0)
                return 0;
    }
}

//calloc_

void*
calloc_(uint size, uint nbytes)
{
  printf(1,"size : %d \n ",size);
  uint total = size * nbytes;
  void* p = cmalloc(total);
  printf(1,"process bytes : %d \n",total);
  if(!p) return 0;
  return memset(p,0,total);
}
