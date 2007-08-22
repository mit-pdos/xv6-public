#include "types.h"
#include "stat.h"
#include "param.h"
#include "mmu.h"
#include "proc.h"
#include "defs.h"
#include "x86.h"
#include "traps.h"
#include "syscall.h"
#include "spinlock.h"
#include "buf.h"
#include "fs.h"
#include "fsvar.h"
#include "elf.h"
#include "file.h"
#include "fcntl.h"

int
exec(char *path, char **argv)
{
  uint sz, sp, p1, p2;
  int i, nargs, argbytes, len, off;
  struct inode *ip;
  struct elfhdr elf;
  struct proghdr ph;
  char *mem;
  char *s, *last;

  sz = 0;
  mem = 0;

  if((ip = ilock(namei(path))) == 0)
    return -1;

  if(readi(ip, (char*)&elf, 0, sizeof(elf)) < sizeof(elf))
    goto bad;

  if(elf.magic != ELF_MAGIC)
    goto bad;

  for(i=0, off=elf.phoff; i<elf.phnum; i++, off+=sizeof(ph)){
    if(readi(ip, (char*)&ph, off, sizeof(ph)) != sizeof(ph))
      goto bad;
    if(ph.type != ELF_PROG_LOAD)
      continue;
    if(ph.memsz < ph.filesz)
      goto bad;
    sz += ph.memsz;
  }

  sz += 4096 - (sz % 4096);
  sz += 4096;

  mem = kalloc(sz);
  if(mem == 0)
    goto bad;
  memset(mem, 0, sz);

  argbytes = 0;
  for(i = 0; argv[i]; i++){
    len = strlen(argv[i]);
    argbytes += len + 1;
  }
  nargs = i;

  // argn\0
  // ...
  // arg0\0
  // 0
  // ptr to argn
  // ...
  // 12: ptr to arg0
  //  8: argv (points to ptr to arg0)
  //  4: argc
  //  0: fake return pc
  sp = sz - argbytes - (nargs+1)*4 - 4 - 4 - 4;
  *(uint*)(mem + sp) = 0xffffffff;
  *(uint*)(mem + sp + 4) = nargs;
  *(uint*)(mem + sp + 8) = (uint)(sp + 12);

  p1 = sp + 12;
  p2 = sp + 12 + (nargs + 1) * 4;
  for(i = 0; i < nargs; i++){
    len = strlen(argv[i]);
    memmove(mem + p2, argv[i], len + 1);
    *(uint*)(mem + p1) = p2;
    p1 += 4;
    p2 += len + 1;
  }
  *(uint*)(mem + p1) = 0;

  // Save name for debugging.
  for(last=s=path; *s; s++)
    if(*s == '/')
      last = s+1;
  safestrcpy(cp->name, last, sizeof(cp->name));

  // commit to the new image.
  kfree(cp->mem, cp->sz);
  cp->sz = sz;
  cp->mem = mem;
  mem = 0;

  for(i=0, off=elf.phoff; i<elf.phnum; i++, off+=sizeof(ph)){
    if(readi(ip, (char*)&ph, off, sizeof(ph)) != sizeof(ph))
      goto bad2;
    if(ph.type != ELF_PROG_LOAD)
      continue;
    if(ph.va + ph.memsz > sz)
      goto bad2;
    if(readi(ip, cp->mem + ph.va, ph.offset, ph.filesz) != ph.filesz)
      goto bad2;
    memset(cp->mem + ph.va + ph.filesz, 0, ph.memsz - ph.filesz);
  }

  iput(iunlock(ip));
  
  cp->tf->eip = elf.entry;
  cp->tf->esp = sp;
  setupsegs(cp);

  return 0;

 bad:
  if(mem)
    kfree(mem, sz);
  iput(iunlock(ip));
  return -1;

 bad2:
  iput(iunlock(ip));
  proc_exit();
  return 0;
}
