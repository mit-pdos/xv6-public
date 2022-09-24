#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "defs.h"
#include "x86.h"
#include "elf.h"
#include "cgroup.h"
#include "vfs_fs.h"
#include "vfs_file.h"
#include "file.h"
#include "defs.h"
#include "kvector.h"

int
exec(char *path, char **argv)
{
  char *s, *last;
  int i, off;
  uint argc, sz, sp, ustack[3+MAXARG+1];
  struct elfhdr elf;
  struct proghdr ph;
  vector elfv, phv;
  struct vfs_inode *ip;
  pde_t *pgdir, *oldpgdir;
  struct proc *curproc = myproc();
  elfv = newvector(sizeof(elf),1);
  phv = newvector(sizeof(ph),1);

  begin_op();

  if((ip = vfs_namei(path)) == 0){
    end_op();
    return -1;
  }
  ip->i_op.ilock(ip);
  pgdir = 0;

  // Check ELF header
  if(ip->i_op.readi(ip, 0, sizeof(elf), &elfv) != sizeof(elf))
    goto bad;

  //vectormemcmp("exec", elfv, 0, (char*)&elf, sizeof(struct elfhdr));
  //Restore data from vector to object
  memmove_from_vector((char*)(&elf), elfv, 0, elfv.vectorsize); // now elfv is the source of elf.

  if(elf.magic != ELF_MAGIC)
    goto bad;

  if((pgdir = setupkvm()) == 0)
    goto bad;

  // Load program into memory.
  sz = 0;
  for(i=0, off=elf.phoff; i<elf.phnum; i++, off+=sizeof(ph)){
    if(ip->i_op.readi(ip, off, sizeof(ph), &phv) != sizeof(ph))
      goto bad;
    memmove_from_vector((char*)(&ph), phv, 0, phv.vectorsize); // now phv is the source of ph.
    if(ph.type != ELF_PROG_LOAD)
      continue;
    if(ph.memsz < ph.filesz)
      goto bad;
    if(ph.vaddr + ph.memsz < ph.vaddr)
      goto bad;
    if((sz = allocuvm(pgdir, sz, ph.vaddr + ph.memsz)) == 0)
      goto bad;
    if(ph.vaddr % PGSIZE != 0)
      goto bad;
    if(loaduvm(pgdir, (char*)ph.vaddr, ip, ph.off, ph.filesz) < 0)
      goto bad;
  }
  ip->i_op.iunlockput(ip);
  end_op();
  ip = 0;

  // Allocate two pages at the next page boundary.
  // Make the first inaccessible.  Use the second as the user stack.
  sz = PGROUNDUP(sz);
  if((sz = allocuvm(pgdir, sz, sz + 2*PGSIZE)) == 0)
    goto bad;
  clearpteu(pgdir, (char*)(sz - 2*PGSIZE));
  sp = sz;

  // Push argument strings, prepare rest of stack in ustack.
  for(argc = 0; argv[argc]; argc++) {
    if(argc >= MAXARG)
      goto bad;
    sp = (sp - (strlen(argv[argc]) + 1)) & ~3;
    if(copyout(pgdir, sp, argv[argc], strlen(argv[argc]) + 1) < 0)
      goto bad;
    ustack[3+argc] = sp;
  }
  ustack[3+argc] = 0;

  ustack[0] = 0xffffffff;  // fake return PC
  ustack[1] = argc;
  ustack[2] = sp - (argc+1)*4;  // argv pointer

  sp -= (3+argc+1) * 4;
  if(copyout(pgdir, sp, ustack, (3+argc+1)*4) < 0)
    goto bad;

  // Save program name for debugging.
  for(last=s=path; *s; s++)
    if(*s == '/')
      last = s+1;
  safestrcpy(curproc->name, last, sizeof(curproc->name));

  // Update memory usage in cgroup and its ancestors
  struct cgroup* cgroup = curproc->cgroup;
  do {
    cgroup->current_mem += sz - curproc->sz;
  } while ((cgroup = cgroup->parent));

  // Commit to the user image.
  oldpgdir = curproc->pgdir;
  curproc->pgdir = pgdir;
  curproc->sz = sz;
  curproc->tf->eip = elf.entry;  // main
  curproc->tf->esp = sp;
  switchuvm(curproc);
  freevm(oldpgdir);
  return 0;

 bad:
  if(pgdir)
    freevm(pgdir);
  if(ip){
    ip->i_op.iunlockput(ip);
    end_op();
  }
  return -1;
}
