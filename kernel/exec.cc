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
#include "kmtrace.hh"

#define BRK (USERTOP >> 1)

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

static int
dosegment(inode* ip, vmap* vmp, u64 off)
{
  struct proghdr ph;
  if(readi(ip, (char*)&ph, off, sizeof(ph)) != sizeof(ph))
    return -1;
  if(ph.type != ELF_PROG_LOAD)
    return -1;
  if(ph.memsz < ph.filesz)
    return -1;
  if (ph.offset < PGOFFSET(ph.vaddr))
    return -1;

  uptr va_start = PGROUNDDOWN(ph.vaddr);
  uptr va_end = PGROUNDUP(ph.vaddr + ph.memsz);
  off_t in_off = ph.offset - PGOFFSET(ph.vaddr);
  size_t in_sz = ph.filesz + PGOFFSET(ph.vaddr);

  size_t npg = (va_end - va_start) / PGSIZE;
  vmnode* node = new vmnode(npg, ONDEMAND, ip, in_off, in_sz);
  if (node == nullptr)
    return -1;

  if (vmp->insert(node, va_start, 1) < 0) {
    delete node;
    return -1;
  }

  return 0;
}

static long
dostack(vmap* vmp, char** argv, const char* path)
{
  struct vmnode *vmn = nullptr;
  uptr argstck[1+MAXARG];
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
    return -1;
  if(vmp->insert(vmn, USERTOP-(USTACKPAGES*PGSIZE), 1) < 0)
    return -1;

  for (argc = 0; argv[argc]; argc++)
    if(argc >= MAXARG)
      return -1;

  // Push argument strings
  sp = USERTOP;
  for(int i = argc-1; i >= 0; i--) {
    sp -= strlen(argv[i]) + 1;
    sp &= ~7;
    if(vmp->copyout(sp, argv[i], strlen(argv[i]) + 1) < 0)
      return -1;
    argstck[i] = sp;
  }
  argstck[argc] = 0;

  sp -= (argc+1) * 8;
  if(vmp->copyout(sp, argstck, (argc+1)*8) < 0)
    return -1;

  sp -= 8;
  if(vmp->copyout(sp, &argc, 8) < 0)
    return -1;

  return sp;
}

static int
doheap(vmap* vmp)
{
  struct vmnode *vmn;

  if((vmn = new vmnode(32)) == nullptr)
    return -1;
  if(vmp->insert(vmn, BRK, 1) < 0)
    return -1;
  vmp->brk_ = BRK + 8;  // XXX so that brk-1 points within heap vma..

  return 0;
}

static void
exec_cleanup(vmap *oldvmap, uwq *olduwq)
{
  if (olduwq != nullptr)
    olduwq->dec();
  oldvmap->decref();
}

int
exec(const char *path, char **argv, void *ascopev)
{
  struct inode *ip = nullptr;
  struct vmap *vmp = nullptr;
  uwq* newuwq = nullptr;
  const char *s, *last;
  struct elfhdr elf;
  struct proghdr ph;
  u64 off;
  int i;
  vmap* oldvmap;
  uwq* olduwq;
  cwork* w;
  long sp;

  myproc()->exec_cpuid_ = mycpuid();

  mt_ascope *ascope = (mt_ascope*) ascopev;
  ascope->close();
  myproc()->in_exec_ = 1;
  yield();
  myproc()->in_exec_ = 0;
  ascope->open("sys_exec2(%s)", path);

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

  if((newuwq = uwq::alloc(vmp, myproc()->ftable)) == 0)
    goto bad;

  for(i=0, off=elf.phoff; i<elf.phnum; i++, off+=sizeof(ph)){
    Elf64_Word type;
    if(readi(ip, (char*)&type, 
             off+__offsetof(struct proghdr, type), 
             sizeof(type)) != sizeof(type))
      goto bad;

    switch (type) {
    case ELF_PROG_NOTE:
      if (donotes(ip, newuwq, off) < 0)
        goto bad;
      break;
    case ELF_PROG_LOAD:
      if (dosegment(ip, vmp, off) < 0)
        goto bad;
      break;
    default:
      continue;
    }
  }

  if (doheap(vmp) < 0)
    goto bad;

  // dostack reads from the user vm space.  wq workers don't switch 
  // the user vm.
  if ((sp = dostack(vmp, argv, path)) < 0)
    goto bad;

  // Commit to the user image.
  oldvmap = myproc()->vmap;
  olduwq = myproc()->uwq;
  myproc()->vmap = vmp;
  myproc()->uwq = newuwq;
  myproc()->tf->rip = elf.entry;
  myproc()->tf->rsp = sp;

  for(last=s=path; *s; s++)
    if(*s == '/')
      last = s+1;
  safestrcpy(myproc()->name, last, sizeof(myproc()->name));

  
  switchvm(myproc());

  w = new cwork();
  assert(w);
  w->rip = (void*) exec_cleanup;
  w->arg0 = oldvmap;
  w->arg1 = olduwq;
  assert(wq_pushto(w, myproc()->exec_cpuid_) >= 0);

  gc_end_epoch();
  return 0;

 bad:
  cprintf("exec failed\n");
  if(vmp)
    vmp->decref();
  if(newuwq)
    newuwq->dec();
  gc_end_epoch();
  return 0;
}
