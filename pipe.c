#include "types.h"
#include "param.h"
#include "x86.h"
#include "mmu.h"
#include "proc.h"
#include "defs.h"
#include "fd.h"

#define PIPESIZE 512

struct pipe {
  int readopen; // read fd is still open
  int writeopen; // write fd is still open
  int writep; // next index to write
  int readp;  // next index to read
  char data[PIPESIZE];
};

int
pipe_alloc(struct fd **fd1, struct fd **fd2)
{
  *fd1 = *fd2 = 0;
  struct pipe *p = 0;

  if((*fd1 = fd_alloc()) == 0)
    goto oops;
  if((*fd2 = fd_alloc()) == 0)
    goto oops;
  if((p = (struct pipe *) kalloc(PAGE)) == 0)
    goto oops;
  (*fd1)->type = FD_PIPE;
  (*fd1)->readable = 1;
  (*fd1)->writeable = 0;
  (*fd1)->pipe = p;
  (*fd2)->type = FD_PIPE;
  (*fd2)->readable = 0;
  (*fd2)->writeable = 1;
  (*fd2)->pipe = p;
  return 0;
 oops:
  if(p)
    kfree((char *) p, PAGE);
  if(*fd1){
    (*fd1)->type = FD_NONE;
    fd_close(*fd1);
  }
  if(*fd2){
    (*fd2)->type = FD_NONE;
    fd_close(*fd2);
  }
  return -1;
}

void
pipe_close(struct pipe *p, int writeable)
{
  if(writeable)
    p->writeopen = 0;
  else
    p->readopen = 0;
  if(p->readopen == 0 && p->writeopen == 0)
    kfree((char *) p, PAGE);
}

int
pipe_write(struct pipe *p, char *addr, int n)
{
  int i;

  for(i = 0; i < n; i++){
    while(((p->writep + 1) % PIPESIZE) == p->readp){
      if(p->readopen == 0)
        return -1;
      sleep(&p->writep);
    }
    p->data[p->writep] = addr[i];
    p->writep = (p->writep + 1) % PIPESIZE;
  }
  return i;
}

int
pipe_read(struct pipe *p, char *addr, int n)
{
  int i;

  while(p->readp == p->writep){
    if(p->writeopen == 0)
      return 0;
    sleep(&p->readp);
  }

  for(i = 0; i < n; i++){
    if(p->readp == p->writep)
      break;
    addr[i] = p->data[p->readp];
    p->readp = (p->readp + 1) % PIPESIZE;
  }
  return i;
}
