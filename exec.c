#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "spinlock.h"
#include "condvar.h"
#include "queue.h"
#include "proc.h"
#include "kernel.h"
#include "x86.h"
#include "stat.h"
#include "fs.h"
#include "file.h"
#include "elf.h"
#include "cpu.h"

#define USTACKPAGES 2

int
exec(char *path, char **argv)
{
  struct inode *ip = NULL;
  struct vmap *vmap = NULL;
  struct vmnode *vmn = NULL;
  struct elfhdr elf;
  struct proghdr ph;
  uptr brk = 0;
  int odp = 1;
  u64 off;
  int i;
  uptr sp;
  int argc;
  uptr ustack[1+MAXARG+1];
  char *s, *last;
  struct vmap *oldvmap;

  if((ip = namei(path)) == 0)
    return -1;

  rcu_begin_read();

  // Check ELF header
  if(ip->type != T_FILE)
    goto bad;
  if(readi(ip, (char*)&elf, 0, sizeof(elf)) < sizeof(elf))
    goto bad;
  if(elf.magic != ELF_MAGIC)
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
    // XXX(sbw) vaddr doesn't have to be page aligned..
    if(ph.vaddr % PGSIZE) {
      cprintf("unaligned ph.va\n");
      goto bad;
    }

    uptr va_start = PGROUNDDOWN(ph.vaddr);
    uptr va_end = PGROUNDUP(ph.vaddr + ph.memsz);
    if(va_end > brk)
      brk = va_end;

    int npg = (va_end - va_start) / PGSIZE;
    if (odp) {
      if ((vmn = vmn_alloc(npg, ONDEMAND)) == 0)
	goto bad;
    } else {
      if ((vmn = vmn_allocpg(npg)) == 0)
	goto bad;
    }

    if(vmn_load(vmn, ip, ph.offset, ph.filesz) < 0)
      goto bad;
    if(vmap_insert(vmap, vmn, ph.vaddr) < 0)
      goto bad;
    vmn = 0;
  }
  if (odp) {
    // iunlock(ip);
  } else {
    // iunlockput(ip);
    iput(ip);
    ip = 0;
  }

  // Allocate a vmnode for the heap.
  // XXX pre-allocate 32 pages..
  if((vmn = vmn_allocpg(32)) == 0)
    goto bad;
  if(vmap_insert(vmap, vmn, brk) < 0)
    goto bad;
  vmn = 0;

  // Allocate a one-page stack at the top of the (user) address space
  if((vmn = vmn_allocpg(USTACKPAGES)) == 0)
    goto bad;
  if(vmap_insert(vmap, vmn, USERTOP-(USTACKPAGES*PGSIZE)) < 0)
    goto bad;
  vmn = 0;

  // Push argument strings, prepare rest of stack in ustack.
  sp = USERTOP;
  for(argc = 0; argv[argc]; argc++) {
    if(argc >= MAXARG)
      goto bad;
    sp -= strlen(argv[argc]) + 1;
    sp &= ~7;
    if(copyout(vmap, sp, argv[argc], strlen(argv[argc]) + 1) < 0)
      goto bad;
    ustack[1+argc] = sp;
  }
  ustack[1+argc] = 0;

  ustack[0] = 0xffffffffffffffffull;  // fake return PC
  myproc()->tf->rdi = argc;
  myproc()->tf->rsi = sp - (argc+1)*8;

  sp -= (1+argc+1) * 8;
  if(copyout(vmap, sp, ustack, (1+argc+1)*8) < 0)
    goto bad;

  // Save program name for debugging.
  for(last=s=path; *s; s++)
    if(*s == '/')
      last = s+1;
  safestrcpy(myproc()->name, last, sizeof(myproc()->name));

  // Commit to the user image.
  oldvmap = myproc()->vmap;
  myproc()->vmap = vmap;
  myproc()->brk = brk + 8;  // XXX so that brk-1 points within heap vma..
  myproc()->tf->rip = elf.entry;  // main
  myproc()->tf->rsp = sp;
  
  switchuvm(myproc());
  vmap_decref(oldvmap);

  migrate(myproc());

  rcu_end_read();
  return 0;

 bad:
  cprintf("exec failed\n");
  if(vmap)
    vmap_decref(vmap);
  if(vmn)
    vmn_free(vmn);
  rcu_end_read();

  return 0;
}
