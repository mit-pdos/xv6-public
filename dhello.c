#include "types.h"
#include "defs.h"
#include "param.h"
#include "traps.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "fs.h"
#include "file.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "x86.h"

#define BUFFERSIZE (8192)

struct hello_pipe {
  struct spinlock lock;
  char data[BUFFERSIZE];
  uint nread;     // number of bytes read
  uint nwrite;    // number of bytes written
};

// Storage for one pipe like buffer
static struct hello_pipe s_hello_pipe;

int
s_hello_pipewrite(struct hello_pipe *p, char *addr, int n)
{
  int i;

  acquire(&p->lock);
  for(i = 0; i < n; i++){
    while(p->nwrite == p->nread + BUFFERSIZE){  //DOC: pipewrite-full
      if(myproc()->killed){
        release(&p->lock);
        return -1;
      }
      wakeup(&p->nread);
      sleep(&p->nwrite, &p->lock);  //DOC: pipewrite-sleep
    }
    p->data[p->nwrite++ % BUFFERSIZE] = addr[i];
  }
  wakeup(&p->nread);  //DOC: pipewrite-wakeup1
  release(&p->lock);
  return n;
}

int
s_hello_piperead(struct hello_pipe *p, char *addr, int n)
{
  int i;

  acquire(&p->lock);
  while(p->nread == p->nwrite){  //DOC: pipe-empty
    if(myproc()->killed){
      release(&p->lock);
      return -1;
    }
    sleep(&p->nread, &p->lock); //DOC: piperead-sleep
  }
  for(i = 0; i < n; i++){  //DOC: piperead-copy
    if(p->nread == p->nwrite)
      break;
    addr[i] = p->data[p->nread++ % BUFFERSIZE];
  }
  wakeup(&p->nwrite);  //DOC: piperead-wakeup
  release(&p->lock);
  return i;
}

int
helloread(struct inode *ip, char *dst, int n) {
    iunlock(ip);
    n = s_hello_piperead(&s_hello_pipe, dst, n);
    ilock(ip);
    return n;
}

int
hellowrite(struct inode *ip, char *src, int n) {
    iunlock(ip);
    n = s_hello_pipewrite(&s_hello_pipe, src, n);
    ilock(ip);
    return n;
}


void
helloinit(void)
{
  //initlock(&cons.lock, "hello");

  devsw[HELLO].write = hellowrite;
  devsw[HELLO].read = helloread;
  //cons.locking = 1;
}

