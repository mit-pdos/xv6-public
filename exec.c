#include "types.h"
#include "param.h"
#include "mmu.h"
#include "proc.h"
#include "defs.h"
#include "x86.h"
#include "elf.h"

int
exec(char *path, char **argv)
{
  char *s, *last;
  int i, off;
  uint sz = 0;
  struct elfhdr elf;
  struct inode *ip = 0;
  struct proghdr ph;
  pde_t *pgdir = 0, *oldpgdir;

  if((ip = namei(path)) == 0)
    return -1;
  ilock(ip);

  // Check ELF header
  if(readi(ip, (char*)&elf, 0, sizeof(elf)) < sizeof(elf))
    goto bad;
  if(elf.magic != ELF_MAGIC)
    goto bad;

  if(!(pgdir = setupkvm()))
    goto bad;

  // Load program into memory.
  for(i=0, off=elf.phoff; i<elf.phnum; i++, off+=sizeof(ph)){
    if(readi(ip, (char*)&ph, off, sizeof(ph)) != sizeof(ph))
      goto bad;
    if(ph.type != ELF_PROG_LOAD)
      continue;
    if(ph.memsz < ph.filesz)
      goto bad;
    if(!(sz = allocuvm(pgdir, sz, ph.va + ph.memsz)))
      goto bad;
    if(!loaduvm(pgdir, (char *)ph.va, ip, ph.offset, ph.filesz))
      goto bad;
  }
  iunlockput(ip);
  ip = 0;

  // Allocate a one-page stack at the next page boundary
  sz = PGROUNDUP(sz);
  if(!(sz = allocuvm(pgdir, sz, sz + PGSIZE)))
    goto bad;

  // initialize stack content:

  // "argumentN"                      -- nul-terminated string
  // ...
  // "argument0"
  // 0                                -- argv[argc]
  // address of argumentN             
  // ...
  // address of argument0             -- argv[0]
  // address of address of argument0  -- argv argument to main()
  // argc                             -- argc argument to main()
  // ffffffff                         -- return PC for main() call

  uint sp = sz;

  // count arguments
  int argc;
  for(argc = 0; argv[argc]; argc++)
    ;
  if(argc >= MAXARG)
    goto bad;

  // push strings and remember where they are
  uint strings[MAXARG];
  for(i = argc - 1; i >= 0; --i){
    sp -= strlen(argv[i]) + 1;
    strings[i] = sp;
    copyout(pgdir, sp, argv[i], strlen(argv[i]) + 1);
  }

#define PUSH(x) { int xx = (int)(x); sp -= 4; copyout(pgdir, sp, &xx, 4); }

  PUSH(0); // argv[argc] is zero

  // push argv[] elements
  for(i = argc - 1; i >= 0; --i)
    PUSH(strings[i]);

  PUSH(sp); // argv

  PUSH(argc);

  PUSH(0xffffffff); // in case main tries to return

  if(sp < sz - PGSIZE)
    goto bad;

  // Save program name for debugging.
  for(last=s=path; *s; s++)
    if(*s == '/')
      last = s+1;
  safestrcpy(proc->name, last, sizeof(proc->name));

  // Commit to the user image.
  oldpgdir = proc->pgdir;
  proc->pgdir = pgdir;
  proc->sz = sz;
  proc->tf->eip = elf.entry;  // main
  proc->tf->esp = sp;

  switchuvm(proc); 

  freevm(oldpgdir);

  return 0;

 bad:
  if(pgdir)
    freevm(pgdir);
  if(ip)
    iunlockput(ip);
  return -1;
}
