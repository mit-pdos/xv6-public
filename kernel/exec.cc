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
#include "wq.hh"
#include "cilk.hh"

#define USTACKPAGES 2
#define BRK (USERTOP >> 1)

struct eargs {
  struct proc *proc;
  struct inode *ip;
  struct vmap *vmap;
  const char *path;
  char **argv;
};

static int
donotes(struct inode *ip, uwq *uwq, u64 off)
{
  struct proghdr ph;
  struct elfnote note;

  if (readi(ip, (char*)&ph, off, sizeof(ph)) != sizeof(ph))
    return -1;
  
  if (readi(ip, (char*)&note, ph.offset, sizeof(note)) != sizeof(note))
    return -1;

  if (note.type == ELF_NOTE_XV6_ADDR) {
    struct xv6_addrdesc desc;

    if (note.descsz != sizeof(desc))
      return -1;
    if (readi(ip, (char*)&desc,
              ph.offset+__offsetof(struct xv6_addrnote, desc),
              sizeof(desc)) != sizeof(desc))
      return -1;

    if (desc.id == XV6_ADDR_ID_WQ) {
      uwq->setuentry(desc.vaddr);
      return 0;
    }
  }
  return -1;
}

static void
dosegment(struct eargs *args, u64 off)
{
  struct vmnode *vmn = nullptr;
  struct proghdr ph;
  uptr va_start, va_end;
  uptr in_off;
  uptr in_sz;
  int npg;

  if(readi(args->ip, (char*)&ph, off, sizeof(ph)) != sizeof(ph))
    goto bad;
  if(ph.type != ELF_PROG_LOAD)
    goto bad;
  if(ph.memsz < ph.filesz)
    goto bad;
  if (ph.offset < PGOFFSET(ph.vaddr))
    goto bad;

  va_start = PGROUNDDOWN(ph.vaddr);
  va_end = PGROUNDUP(ph.vaddr + ph.memsz);
  in_off = ph.offset - PGOFFSET(ph.vaddr);
  in_sz = ph.filesz + PGOFFSET(ph.vaddr);

  npg = (va_end - va_start) / PGSIZE;
  if ((vmn = new vmnode(npg, ONDEMAND,
                        args->ip, in_off, in_sz)) == 0)
    goto bad;

  if(args->vmap->insert(vmn, va_start, 1) < 0)
    goto bad;

  return;
  
bad:
  cilk_abort(-1);
}

static void
dostack(struct eargs *args)
{
  struct vmnode *vmn = nullptr;
  uptr argstck[1+MAXARG];
  const char *s, *last;
  s64 argc;
  uptr sp;

  // User stack should be:
  //   char argv[argc-1]
  //   char argv[argc-2]
  //   ...
  //   char argv[0]
  //   char *argv[argc+1]
  //   u64 argc

  // Allocate a one-page stack at the top of the (user) address space
  if((vmn = new vmnode(USTACKPAGES)) == 0)
    goto bad;
  if(args->vmap->insert(vmn, USERTOP-(USTACKPAGES*PGSIZE), 1) < 0)
    goto bad;

  for (argc = 0; args->argv[argc]; argc++)
    if(argc >= MAXARG)
      goto bad;

  // Push argument strings
  sp = USERTOP;
  for(int i = argc-1; i >= 0; i--) {
    sp -= strlen(args->argv[i]) + 1;
    sp &= ~7;
    if(args->vmap->copyout(sp, args->argv[i], strlen(args->argv[i]) + 1) < 0)
      goto bad;
    argstck[i] = sp;
  }
  argstck[argc] = 0;

  sp -= (argc+1) * 8;
  if(args->vmap->copyout(sp, argstck, (argc+1)*8) < 0)
    goto bad;

  sp -= 8;
  if(args->vmap->copyout(sp, &argc, 8) < 0)
    goto bad;

  // Save program name for debugging.
  for(last=s=args->path; *s; s++)
    if(*s == '/')
      last = s+1;

  // XXX(sbw) Oops, don't want to do this, unless we have abort
  safestrcpy(args->proc->name, last, sizeof(args->proc->name));
  args->proc->tf->rsp = sp;

  return;

bad:
  cilk_abort(-1);
}

static void 
doheap(struct eargs *args)
{
  struct vmnode *vmn = nullptr;

  // Allocate a vmnode for the heap.
  // XXX pre-allocate 32 pages..
  if((vmn = new vmnode(32)) == 0)
    goto bad;
  if(args->vmap->insert(vmn, BRK, 1) < 0)
    goto bad;
  args->vmap->brk_ = BRK + 8;  // XXX so that brk-1 points within heap vma..

  return;

bad:
  cilk_abort(-1);
}

int
exec(const char *path, char **argv)
{
  struct inode *ip = nullptr;
  struct vmap *vmp = nullptr;
  uwq* uwq = nullptr;
  struct elfhdr elf;
  struct proghdr ph;
  u64 off;
  int i;
  struct vmap *oldvmap;
  
  if((ip = namei(myproc()->cwd, path)) == 0)
    return -1;

  if(myproc()->worker != nullptr)
    return -1;

  gc_begin_epoch();

  // Check ELF header
  if(ip->type != T_FILE)
    goto bad;
  if(readi(ip, (char*)&elf, 0, sizeof(elf)) < sizeof(elf))
    goto bad;
  if(elf.magic != ELF_MAGIC)
    goto bad;

  if((vmp = vmap::alloc()) == 0)
    goto bad;

  if((uwq = uwq::alloc(vmp, myproc()->ftable)) == 0)
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
    if (type == ELF_PROG_NOTE) {
      if (donotes(ip, uwq, off) < 0) {
        cilk_abort(-1);
        break;
      }
    } if(type != ELF_PROG_LOAD)
      continue;
    cilk_call(dosegment, &args, off);
  }

  cilk_call(doheap, &args);

  // dostack reads from the user vm space.  wq workers don't switch 
  // the user vm.
  dostack(&args);

  if (cilk_end())
    goto bad;

  // Commit to the user image.
  oldvmap = myproc()->vmap;
  myproc()->vmap = vmp;
  if (myproc()->uwq != nullptr)
    myproc()->uwq->dec();
  myproc()->uwq = uwq;
  myproc()->tf->rip = elf.entry;
  
  switchvm(myproc());
  oldvmap->decref();

  gc_end_epoch();
  return 0;

 bad:
  cprintf("exec failed\n");
  if(vmp)
    vmp->decref();
  if(uwq)
    uwq->dec();
  gc_end_epoch();
  return 0;
}
