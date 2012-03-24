#include "types.h"
#include "mmu.h"
#include "kernel.hh"
#include "spinlock.h"
#include "condvar.h"
#include "queue.h"
#include "proc.hh"
#include "fs.h"
#include "file.hh"
#include "cpu.hh"

#define PIPESIZE 512

struct pipe {
  struct spinlock lock;
  struct condvar  cv;
  char data[PIPESIZE];
  u32 nread;      // number of bytes read
  u32 nwrite;     // number of bytes written
  int readopen;   // read fd is still open
  int writeopen;  // write fd is still open
};

int
pipealloc(struct file **f0, struct file **f1)
{
  struct pipe *p;

  p = 0;
  *f0 = *f1 = 0;
  if((*f0 = file::alloc()) == 0 || (*f1 = file::alloc()) == 0)
    goto bad;
  if((p = (pipe*)kmalloc(sizeof(*p), "pipe")) == 0)
    goto bad;
  p->readopen = 1;
  p->writeopen = 1;
  p->nwrite = 0;
  p->nread = 0;
  initlock(&p->lock, "pipe", LOCKSTAT_PIPE);
  initcondvar(&p->cv, "pipe");
  (*f0)->type = file::FD_PIPE;
  (*f0)->readable = 1;
  (*f0)->writable = 0;
  (*f0)->pipe = p;
  (*f1)->type = file::FD_PIPE;
  (*f1)->readable = 0;
  (*f1)->writable = 1;
  (*f1)->pipe = p;
  return 0;

//PAGEBREAK: 20
 bad:
  if(p) {
    destroylock(&p->lock);
    kmfree((char*)p, sizeof(*p));
  }
  if(*f0)
    (*f0)->dec();
  if(*f1)
    (*f1)->dec();
  return -1;
}

void
pipeclose(struct pipe *p, int writable)
{
  acquire(&p->lock);
  if(writable){
    p->writeopen = 0;
  } else {
    p->readopen = 0;
  }
  cv_wakeup(&p->cv);
  if(p->readopen == 0 && p->writeopen == 0){
    release(&p->lock);
    destroylock(&p->lock);
    kmfree((char*)p, sizeof(*p));
  } else
    release(&p->lock);
}

//PAGEBREAK: 40
int
pipewrite(struct pipe *p, char *addr, int n)
{
  int i;

  acquire(&p->lock);
  for(i = 0; i < n; i++){
    while(p->nwrite == p->nread + PIPESIZE){  //DOC: pipewrite-full
      if(p->readopen == 0 || myproc()->killed){
        release(&p->lock);
        return -1;
      }
      cv_wakeup(&p->cv);
      cv_sleep(&p->cv, &p->lock); //DOC: pipewrite-sleep
    }
    p->data[p->nwrite++ % PIPESIZE] = addr[i];
  }
  cv_wakeup(&p->cv);  //DOC: pipewrite-wakeup1
  release(&p->lock);
  return n;
}

int
piperead(struct pipe *p, char *addr, int n)
{
  int i;

  acquire(&p->lock);
  while(p->nread == p->nwrite && p->writeopen){  //DOC: pipe-empty
    if(myproc()->killed){
      release(&p->lock);
      return -1;
    }
    cv_sleep(&p->cv, &p->lock); //DOC: piperead-sleep
  }
  for(i = 0; i < n; i++){  //DOC: piperead-copy
    if(p->nread == p->nwrite)
      break;
    addr[i] = p->data[p->nread++ % PIPESIZE];
  }
  cv_wakeup(&p->cv);  //DOC: piperead-wakeup
  release(&p->lock);
  return i;
}
