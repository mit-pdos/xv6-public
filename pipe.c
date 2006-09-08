#include "types.h"
#include "param.h"
#include "x86.h"
#include "mmu.h"
#include "proc.h"
#include "defs.h"
#include "file.h"
#include "spinlock.h"

#define PIPESIZE 512

struct pipe {
  int readopen; // read fd is still open
  int writeopen; // write fd is still open
  int writep; // next index to write
  int readp;  // next index to read
  struct spinlock lock;
  char data[PIPESIZE];
};

int
pipe_alloc(struct file **f0, struct file **f1)
{
  *f0 = *f1 = 0;
  struct pipe *p = 0;

  if((*f0 = filealloc()) == 0)
    goto oops;
  if((*f1 = filealloc()) == 0)
    goto oops;
  if((p = (struct pipe*) kalloc(PAGE)) == 0)
    goto oops;
  p->readopen = 1;
  p->writeopen = 1;
  p->writep = 0;
  p->readp = 0;
  initlock(&p->lock, "pipe");
  (*f0)->type = FD_PIPE;
  (*f0)->readable = 1;
  (*f0)->writable = 0;
  (*f0)->pipe = p;
  (*f1)->type = FD_PIPE;
  (*f1)->readable = 0;
  (*f1)->writable = 1;
  (*f1)->pipe = p;
  return 0;
 oops:
  if(p)
    kfree((char*) p, PAGE);
  if(*f0){
    (*f0)->type = FD_NONE;
    fileclose(*f0);
  }
  if(*f1){
    (*f1)->type = FD_NONE;
    fileclose(*f1);
  }
  return -1;
}

void
pipe_close(struct pipe *p, int writable)
{
  acquire(&p->lock);

  if(writable){
    p->writeopen = 0;
    wakeup(&p->readp);
  } else {
    p->readopen = 0;
    wakeup(&p->writep);
  }

  release(&p->lock);

  if(p->readopen == 0 && p->writeopen == 0)
    kfree((char*) p, PAGE);
}

int
pipe_write(struct pipe *p, char *addr, int n)
{
  int i;

  acquire(&p->lock);

  for(i = 0; i < n; i++){
    while(((p->writep + 1) % PIPESIZE) == p->readp){
      if(p->readopen == 0){
        release(&p->lock);
        return -1;
      }
      wakeup(&p->readp);
      sleep(&p->writep, &p->lock);
    }
    p->data[p->writep] = addr[i];
    p->writep = (p->writep + 1) % PIPESIZE;
  }

  release(&p->lock);
  wakeup(&p->readp);
  return i;
}

int
pipe_read(struct pipe *p, char *addr, int n)
{
  int i;

  acquire(&p->lock);

  while(p->readp == p->writep){
    if(p->writeopen == 0){
      release(&p->lock);
      return 0;
    }
    sleep(&p->readp, &p->lock);
  }

  for(i = 0; i < n; i++){
    if(p->readp == p->writep)
      break;
    addr[i] = p->data[p->readp];
    p->readp = (p->readp + 1) % PIPESIZE;
  }

  release(&p->lock);
  wakeup(&p->writep);
  return i;
}
