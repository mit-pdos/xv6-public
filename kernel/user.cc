#include "types.h"
#include "kernel.hh"
#include "mmu.h"
#include "amd64.h"
#include "spinlock.h"
#include "condvar.h"
#include "queue.h"
#include "proc.hh"
#include "cpu.hh"
#include "bits.hh"
#include "vm.hh"

#define INIT_START 0x1000

extern struct proc *bootproc;

// Set up first user process.
void
inituser(void)
{
  struct proc *p;
  extern u8 _initcode_start[];
  extern u64 _initcode_size;

  p = proc::alloc();
  p->ftable = filetable::alloc();
  if (p->ftable == nullptr)
    panic("userinit: new filetable");
  bootproc = p;
  if((p->vmap = vmap::alloc()) == 0)
    panic("userinit: out of vmaps?");
  if ((p->pgmap = proc_pgmap::alloc(p->vmap)) == 0)
    panic("inituser: alloc proc_pgmap");
  vmnode *vmn =  new vmnode(PGROUNDUP(_initcode_size) / PGSIZE);
  if(vmn == 0)
    panic("userinit: vmn_allocpg");
  if(p->vmap->insert(vmn, INIT_START, 1, nullptr) < 0)
    panic("userinit: vmap_insert");
  if(p->vmap->copyout(INIT_START, _initcode_start, _initcode_size) < 0)
    panic("userinit: copyout");
  memset(p->tf, 0, sizeof(*p->tf));
  p->tf->cs = UCSEG | 0x3;
  p->tf->ds = UDSEG | 0x3;
  p->tf->ss = p->tf->ds;
  p->tf->rflags = FL_IF;
  p->tf->rsp = PGSIZE;
  p->tf->rip = INIT_START;  // beginning of initcode.S
  p->data_cpuid = myid();

  safestrcpy(p->name, "initcode", sizeof(p->name));
  p->cwd = 0; // forkret will fix in the process's context
  acquire(&p->lock);
  addrun(p);
  release(&p->lock);
}
