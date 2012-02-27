#include "types.h"
#include "mmu.h"
#include "kernel.hh"
#include "spinlock.h"
#include "condvar.h"
#include "queue.h"
#include "proc.hh"
#include "amd64.h"
#include "stat.h"
#include "fs.h"
#include "file.hh"
#include "vm.hh"
#include "elf.hh"
#include "cpu.hh"
#include "prof.hh"

#define USTACKPAGES 2
#define BRK (USERTOP >> 1)

static const int odp = 1;

struct eargs {
  struct proc *proc;
  struct inode *ip;
  struct vmap *vmap;
  const char *path;
  char **argv;
};

static void
dosegment(uptr a0, u64 a1)
{
  struct eargs *args = (eargs*) a0;
  u64 off = a1;
  struct vmnode *vmn = nullptr;
  struct proghdr ph;

  prof_start(dosegment_prof);
  if(readi(args->ip, (char*)&ph, off, sizeof(ph)) != sizeof(ph))
    goto bad;
  if(ph.type != ELF_PROG_LOAD)
    goto bad;
  if(ph.memsz < ph.filesz)
    goto bad;
  // XXX(sbw) vaddr doesn't have to be page aligned..
  if(ph.vaddr % PGSIZE) {
    cprintf("unaligned ph.va\n");
    goto bad;
  }

  {
    uptr va_start = PGROUNDDOWN(ph.vaddr);
    uptr va_end = PGROUNDUP(ph.vaddr + ph.memsz);
  
    int npg = (va_end - va_start) / PGSIZE;
    if ((vmn = new vmnode(npg, odp ? ONDEMAND : EAGER,
                          args->ip, ph.offset, ph.filesz)) == 0)
      goto bad;

    if(args->vmap->insert(vmn, ph.vaddr, 1) < 0)
      goto bad;

    prof_end(dosegment_prof);
    return;
  }
  
bad:
  panic("dosegment: Oops");
}

static void dostack(uptr a0, u64 a1)
{
  struct vmnode *vmn = nullptr;
  struct eargs *args = (eargs*) a0;  
  int argc;
  uptr sp;
  uptr ustack[1+MAXARG+1];
  const char *s, *last;

  prof_start(dostack_prof);
    // Allocate a one-page stack at the top of the (user) address space
  if((vmn = new vmnode(USTACKPAGES)) == 0)
    goto bad;
  if(args->vmap->insert(vmn, USERTOP-(USTACKPAGES*PGSIZE), 1) < 0)
    goto bad;

  // Push argument strings, prepare rest of stack in ustack.
  sp = USERTOP;
  for(argc = 0; args->argv[argc]; argc++) {
    if(argc >= MAXARG)
      goto bad;
    sp -= strlen(args->argv[argc]) + 1;
    sp &= ~7;
    if(args->vmap->copyout(sp, args->argv[argc], strlen(args->argv[argc]) + 1) < 0)
      goto bad;
    ustack[1+argc] = sp;
  }
  ustack[1+argc] = 0;

  //prof_start(exec3_prof);
  ustack[0] = 0xffffffffffffffffull;  // fake return PC
  args->proc->tf->rdi = argc;
  args->proc->tf->rsi = sp - (argc+1)*8;

  sp -= (1+argc+1) * 8;
  if(args->vmap->copyout(sp, ustack, (1+argc+1)*8) < 0)
    goto bad;

  // Save program name for debugging.
  for(last=s=args->path; *s; s++)
    if(*s == '/')
      last = s+1;
  safestrcpy(args->proc->name, last, sizeof(args->proc->name));

  args->proc->tf->rsp = sp;

  prof_end(dostack_prof);
  return;

bad:
  panic("dostack: Oops");
}

static void doheap(uptr a0, u64 a1)
{
  struct vmnode *vmn = nullptr;
  struct eargs *args = (eargs*) a0;

  prof_start(doheap_prof);
  // Allocate a vmnode for the heap.
  // XXX pre-allocate 32 pages..
  if((vmn = new vmnode(32)) == 0)
    goto bad;
  if(args->vmap->insert(vmn, BRK, 1) < 0)
    goto bad;
  prof_end(doheap_prof);

  return;

bad:
  panic("doheap: Oops");
}

int
exec(const char *path, char **argv)
{
  struct inode *ip = nullptr;
  struct vmap *vmp = nullptr;
  struct elfhdr elf;
  struct proghdr ph;
  u64 off;
  int i;
  struct vmap *oldvmap;

  prof_start(exec_prof);
  if((ip = namei(path)) == 0)
    return -1;

  gc_begin_epoch();

  // Check ELF header
  if(ip->type != T_FILE)
    goto bad;
  if(readi(ip, (char*)&elf, 0, sizeof(elf)) < sizeof(elf))
    goto bad;
  if(elf.magic != ELF_MAGIC)
    goto bad;

  if((vmp = new vmap()) == 0)
    goto bad;

  // Arguments for work queue
  struct eargs args;
  args.proc = myproc();
  args.ip = ip;
  args.vmap = vmp;
  args.path = path;
  args.argv = argv;

  cilk_start();
  for(i=0, off=elf.phoff; i<elf.phnum; i++, off+=sizeof(ph)){
    Elf64_Word type;
    if(readi(ip, (char*)&type, 
             off+__offsetof(struct proghdr, type), 
             sizeof(type)) != sizeof(type))
      goto bad;
    if(type != ELF_PROG_LOAD)
      continue;
    cilk_push(dosegment, (uptr)&args, (uptr)off);
  }

  if (odp) {
    // iunlock(ip);
  } else {
    // iunlockput(ip);
    iput(ip);
    ip = 0;
  }

  cilk_push(doheap, (uptr)&args, (uptr)0);

  // dostack reads from the user address space.  The wq
  // stuff doesn't switch to the user address space.
  //cilk_push(dostack, (uptr)&args, (uptr)0);
  dostack((uptr)&args, (uptr)0);

  cilk_end();

  // Commit to the user image.
  oldvmap = myproc()->vmap;
  myproc()->vmap = vmp;
  myproc()->brk = BRK + 8;  // XXX so that brk-1 points within heap vma..
  myproc()->tf->rip = elf.entry;  // main
  
  switchvm(myproc());
  oldvmap->decref();

  gc_end_epoch();
  prof_end(exec_prof);
  return 0;

 bad:
  cprintf("exec failed\n");
  if(vmp)
    vmp->decref();
  gc_end_epoch();

  return 0;
}
