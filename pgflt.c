#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "x86.h"
#include "proc.h"
#include "swap.h"

// Fetches the struct loadseg which spans across the
// faulting address.
/* struct loadseg contains a subset of the fields present in
  loadable ELF program headers. It houses only those fields
  which are useful to the loader.*/
static struct loadseg*
fetch_loadseg(uint flt_addr, struct elfprof *ep)
{
  while(ep && ep->start_vaddr > flt_addr)
    ep = ep->next;

  if(!ep)
    return 0;

  for(int i = 0; i < ep->numseg; i++) {
    if(ep->ls[i].vaddr <= flt_addr && ((ep->ls[i].vaddr + ep->ls[i].memsz) >= flt_addr))
      return &ep->ls[i];
  }
  return 0;
}

// The loader
/* Loads atmost PGSIZE amount of content from the ELF file into
  the RAM at a specified physical address.*/
static int
loadpage(struct inode *ip, uint flt_addr, char *pa, struct loadseg *ls)
{
  // If issued address lies between vaddr + filesz and
  // vaddr + memsz, nothing to load here. Simply return.
  if(PGROUNDDOWN(flt_addr) >= ls->vaddr + ls->filesz)
    return 0;

  // If size of content to be loaded in less than PGSIZE,
  // consider sz accordingly.
  uint sz = ls->vaddr + ls->filesz - PGROUNDDOWN(flt_addr);
  sz = sz < PGSIZE ? sz : PGSIZE;

  // Get the ELF offset.
  uint off = ls->off + PGROUNDDOWN(flt_addr) - ls->vaddr;

  begin_op();
  ilock(ip);
  if(readi(ip, pa, off, sz) != sz) {
    iunlock(ip);
    end_op();
    return -1;
  }
  iunlock(ip);
  end_op();
  return 0;
}

// Checks if the page fault was triggered due to an
// illegal guard range access.
static int
check_stack_access(struct elfprof *ep, uint flt_addr)
{
  uint guard_range = PGROUNDUP(ep->end_vaddr);
  uint stack_pg = guard_range + PGSIZE;

  if(flt_addr < guard_range || flt_addr >= stack_pg)
    return 0;
  return -1;
}

void
page_fault_handler(void)
{
  char *flt_addr, *empty_page = 0;

  // Get the faulting address.
  /* In x86, the faulting address is stored
    in CR2 register.*/
  flt_addr = (char *)rcr2();

  // Get the current process' proc struct.
  struct proc *p = myproc();

  // Check for illegal access.
  /* Any addresses issued over p->sz is treated
    as an illegal access.*/
	if((uint)flt_addr > p->sz)
	  goto illegal_access;

	// Check for stack access (already allocated in exec)
	if((check_stack_access(p->ep, (uint)flt_addr)) < 0)
	  goto illegal_access;

  // Allocate a physical page for the process
  // from memory.
  if((empty_page = kalloc()) == 0)
    goto out_of_memory;
  // Set all bytes in the page to 0.
  /* Just to make life easier, will refactor
    later if necessary.*/
  memset(empty_page, 0, PGSIZE);

  // Check if page was swapped out before.
  pte_t *pte = walkpgdir(p->pgdir, flt_addr, 0);
  if(pte && PTE_ADDR(*pte)) {
    uint swapslot = GET_SWAPSLOT(*pte);
    read_from_swap(swapslot, empty_page);
  }

  // Map the allocated page's physical address
  // to user's virtual address space.
  mappages(p->pgdir, flt_addr, PGSIZE, V2P(empty_page), PTE_P|PTE_W|PTE_U, p->pid);

  // If the issued address was intended to access the heap, return
  // as we have already mapped an empty page to the page table entry
  // corresponding to the faulting address.
  // If it was a stack access, also return as we have loaded from swap.
  if((uint)flt_addr >= PGROUNDUP(p->ep->end_vaddr) + PGSIZE)
    return;

  // Fetch the information (mainly the ELF offset and size)
  // of the loadable segment which spans across the faulting
  // address.
  // If not, terminate the process as it's an illegal access.
  struct loadseg *ls = fetch_loadseg((uint)flt_addr, p->ep);
  if(!ls)
    goto illegal_access;

  // Load the contents from the ELF file into the empty_page allocated
  // before in physical memory.
  if(loadpage(p->elf, (uint)flt_addr, empty_page, ls) < 0)
    goto load_fail;
  return;

  illegal_access:
    cprintf("illegal memory access, process killed.\n");
		goto kill_process;

  load_fail:
    cprintf("can't load content from disk, process killed.\n");
    goto kill_process;

  out_of_memory:
    cprintf("out of memory, process killed\n");
    goto kill_process;

  kill_process:
    if(empty_page)
      kfree(empty_page);
    p->killed = 1;
		return;
}
