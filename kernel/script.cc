#include "types.h"
#include "kernel.hh"
#include "spinlock.h"
#include "condvar.h"
#include "cpu.hh"
#include "proc.hh"
#include "fs.h"
#include "wq.hh"
#include "ipc.hh"
#include "vm.hh"
#include "file.hh"

void
script_mmap_work(void *a0, void *a1, void *a2, void *a3)
{
  void *addr = (void *) a0;
  u64 len = (u64) a1;
  atomic<int> *donep = (atomic<int> *) a2;
  struct proc *p = (struct proc *) a3;

  vmnode *vmn = new vmnode(PGROUNDUP(len) / PGSIZE);
  if(vmn == 0)
    panic("sys_script: new vmnode");

  if(p->vmap->insert(vmn, PGROUNDDOWN((u64)addr), 0, p->pgmap) < 0)
    panic("sys_script: insert");

  *donep += 1;
}

//SYSCALL
int
sys_script(void *addr, u64 len, u64 chunk)
{
  atomic<int> done;
  done = 0;
  int queued = 0;
  char *q = (char*) addr;
  while(q < (char*)addr + len){
    u64 x = len - (q - (char*)addr);
    if(x > chunk)
      x = chunk;

    cwork *w = new cwork();
    if(w == 0)
      panic("sys_script allocwork");
    w->rip = (void*) script_mmap_work;
    w->arg0 = (void *) q;
    w->arg1 = (void *) x;
    w->arg2 = (void *) &done;
    w->arg3 = (void *) myproc();
    if(wq_push(w) < 0)
      panic("sys_script wq_push");
    queued++;

    q += chunk;
  }

  while(done < queued){
    // yield();
    wq_trywork();
  }

  tlbflush();

  return 0;
}
