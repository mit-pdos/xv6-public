#include "types.h"
#include "param.h"
#include "mmu.h"
#include "spinlock.h"
#include "condvar.h"
#include "queue.h"
#include "proc.h"
#include "defs.h"
#include "x86.h"
#include "elf.h"

int
exec(char *path, char **argv)
{
  char *s, *last;
  int i, off, brk = 0;
  uint argc, sp, ustack[3+MAXARG+1];
  struct elfhdr elf;
  struct inode *ip = 0;
  struct proghdr ph;
  pde_t *pgdir = 0, *oldpgdir;
  struct vmap *vmap = 0, *oldvmap;
  struct vmnode *vmn = 0;

  if((ip = namei(path)) == 0)
    return -1;
  ilock(ip);
  pgdir = 0;

  // Check ELF header
  if(readi(ip, (char*)&elf, 0, sizeof(elf)) < sizeof(elf))
    goto bad;
  if(elf.magic != ELF_MAGIC)
    goto bad;

  if((pgdir = setupkvm()) == 0)
    goto bad;

  if((vmap = vmap_alloc()) == 0)
    goto bad;

  // Load program into memory.
  for(i=0, off=elf.phoff; i<elf.phnum; i++, off+=sizeof(ph)){
    if(readi(ip, (char*)&ph, off, sizeof(ph)) != sizeof(ph))
      goto bad;
    if(ph.type != ELF_PROG_LOAD)
      continue;
    if(ph.memsz < ph.filesz)
      goto bad;
    if(ph.va % PGSIZE) {
      cprintf("unaligned ph.va\n");
      goto bad;
    }

    uint va_start = PGROUNDDOWN(ph.va);
    uint va_end = PGROUNDUP(ph.va + ph.memsz);
    if(va_end > brk)
      brk = va_end;

    int npg = (va_end - va_start) / PGSIZE;
    if ((vmn = vmn_allocpg(npg)) == 0)
      goto bad;
    if(vmn_load(vmn, ip, ph.offset, ph.filesz) < 0)
      goto bad;
    if(vmap_insert(vmap, vmn, ph.va) < 0)
      goto bad;
    vmn = 0;
  }
  iunlockput(ip);
  ip = 0;

  // Allocate a vmnode for the heap.
  // XXX pre-allocate 32 pages..
  if((vmn = vmn_allocpg(32)) == 0)
    goto bad;
  if(vmap_insert(vmap, vmn, brk) < 0)
    goto bad;
  vmn = 0;

  // Allocate a one-page stack at the top of the (user) address space
  if((vmn = vmn_allocpg(1)) == 0)
    goto bad;
  if(vmap_insert(vmap, vmn, USERTOP-PGSIZE) < 0)
    goto bad;
  vmn = 0;

  // Push argument strings, prepare rest of stack in ustack.
  sp = USERTOP;
  for(argc = 0; argv[argc]; argc++) {
    if(argc >= MAXARG)
      goto bad;
    sp -= strlen(argv[argc]) + 1;
    sp &= ~3;
    if(copyout(vmap, sp, argv[argc], strlen(argv[argc]) + 1) < 0)
      goto bad;
    ustack[3+argc] = sp;
  }
  ustack[3+argc] = 0;

  ustack[0] = 0xffffffff;  // fake return PC
  ustack[1] = argc;
  ustack[2] = sp - (argc+1)*4;  // argv pointer

  sp -= (3+argc+1) * 4;
  if(copyout(vmap, sp, ustack, (3+argc+1)*4) < 0)
    goto bad;

  // Save program name for debugging.
  for(last=s=path; *s; s++)
    if(*s == '/')
      last = s+1;
  safestrcpy(proc->name, last, sizeof(proc->name));

  // Commit to the user image.
  oldpgdir = proc->pgdir;
  oldvmap = proc->vmap;
  proc->pgdir = pgdir;
  proc->vmap = vmap;
  proc->brk = brk + 4;  // XXX so that brk-1 points within heap vma..
  proc->tf->eip = elf.entry;  // main
  proc->tf->esp = sp;
  switchuvm(proc);
  freevm(oldpgdir);
  vmap_decref(oldvmap);

  return 0;

 bad:
  cprintf("exec failed\n");
  if(pgdir)
    freevm(pgdir);
  if(ip)
    iunlockput(ip);
  if(vmap)
    vmap_decref(vmap);
  if(vmn)
    vmn_free(vmn);
  return -1;
}
