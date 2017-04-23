// ------------ Includes ---------------

#include "types.h"
#include "defs.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "param.h"
#include "mmu.h"
#include "fs.h"
#include "buf.h"
#include "file.h"
#include "memlayout.h"
#include "proc.h"

// ----------- Definitions -------------

void bfree(uint b);
void pffree(struct dinode *dip);
struct pfile* pfalloc(int pid);
struct pfile* pfget(int pid);
void storesegments(struct dinode *dip, int pageindex, uint *pgaddr);
void loadsegments(struct dinode *dip, int pageindex, uint *pgaddr);
void settime(struct proc *p, uint *pageaddr);
void cleartime(struct proc *p, uint *pageaddr);
uint gettime(struct proc *p, uint *pageaddr);


struct pfile {
  struct dinode * dip;
  int pid;
};

// -------- Global Variables -----------

int dev = ROOTDEV;
struct {
  struct spinlock lock;
  struct pfile pfiles[NPROC];
} pftable;

// --------- Public Methods ------------

uint *
findoldpage(struct proc *p)
{
  uint *page, *temp;
  int i, j;
  
  temp = FINDPAGE(p->pgdir, 0, 0);
  
  for(i = 0; i < NPDENTRIES; i++){
    for(j = 0; j < NPTENTRIES; j++){
      page = FINDPAGE(p->pgdir, i, j);

      if((*page & PTE_P) && !(*page & PTE_PG)){
        if(SELECTION == NFU)
          temp = (*page & PTE_A) ? temp : page;
        if(SELECTION == FIFO)
          temp = (gettime(p, page) > gettime(p, temp)) ? temp : page;
      }
    }
  }

  return temp;
}

void
pfcopy(struct proc *parent, struct proc *p)
{
  int inum;
  struct buf *bp;
  struct dinode *dip;
  struct pfile *pf, *parentpf;

  for(inum = 1; inum < sb.ninodes; inum++) {
    bp = bread(dev, IBLOCK(inum, sb));
    dip = (struct dinode*)bp->data + inum%IPB;

    if(dip->type == 0) {
      if((parentpf = pfget(parent->pid)) > 0){
        memmove(dip, parentpf->dip, sizeof(*dip));
        memmove(p->pageaddrs, parent->pageaddrs, sizeof(p->pageaddrs));
        memmove(p->pagetimes, parent->pagetimes, sizeof(p->pagetimes));
      }
      brelse(bp);
      pf = pfalloc(p->pid);
      pf->dip = dip;
      return;
    }

    brelse(bp);
  }
  panic("pfcreate: no inodes");
}

void
storepage(struct proc *p, uint *pgaddr)
{
  struct pfile *pf;
  struct buf *bp;
  int i;

  if((pf = pfget(p->pid)) < 0)
    panic("strorepage: pfile not found");
  
  acquire(&pftable.lock);
  bp = bread(dev, pf->dip->addrs[0]);

  for(i = 0; i < MAX_TOTAL_PAGES - MAX_PSYC_PAGES + 1; i++){
    if(!bp->data[i]){
      bp->data[i] = (uint)pgaddr;
      storesegments(pf->dip, i, pgaddr);
      cleartime(p, pgaddr);
      brelse(bp);
      release(&pftable.lock);
      return;
    }
  }

  brelse(bp);
  release(&pftable.lock);
  panic("storepage: out of space");
}

void
loadpage(struct proc *p, uint *pgaddr)
{
  struct pfile *pf;
  struct buf *bp;
  int i;

  if((pf = pfget(p->pid)) < 0)
    panic("loadpage: pfile not found");

  acquire(&pftable.lock);
  bp = bread(dev, pf->dip->addrs[0]);

  for(i = 0; i < MAX_TOTAL_PAGES - MAX_PSYC_PAGES + 1; i++){
    if(bp->data[i] == (uint)pgaddr){
      bp->data[i] = 0;
      loadsegments(pf->dip, i, pgaddr);
      settime(p, pgaddr);
      brelse(bp);
      release(&pftable.lock);
      return;
    }
  }

  brelse(bp);
  release(&pftable.lock);
  panic("loadpage: page not found");
}

void
deletepage(struct proc *p, uint *pgaddr)
{
  struct pfile *pf;
  struct buf *bp;
  int i;

  if((pf = pfget(p->pid)) < 0)
    panic("loadpage: pfile not found");

  acquire(&pftable.lock);
  bp = bread(dev, pf->dip->addrs[0]);

  for(i = 0; i < MAX_TOTAL_PAGES - MAX_PSYC_PAGES + 1; i++){
    if(bp->data[i] == (uint)pgaddr){
      bp->data[i] = 0;
      loadsegments(pf->dip, i, pgaddr);
      settime(p, pgaddr);
      brelse(bp);
      release(&pftable.lock);
      return;
    }
  }

  brelse(bp);
  release(&pftable.lock);
  panic("deletepage: page not found");
}

void
pfdelete(int pid)
{
  struct pfile *pf;
  acquire(&pftable.lock);

  for(pf = pftable.pfiles; pf < &pftable.pfiles[NPROC]; pf++){
    if(pf->pid){
      pffree(pf->dip);
      pf->pid = 0;
      pf->dip = 0;
      release(&pftable.lock);
      return;
    }
  }

  release(&pftable.lock);
  panic("pfdelete: pf not found");
}

int
getpagecount(struct proc *p)
{
  int i, count;
  for(i = count = 0; i < MAX_PSYC_PAGES; i++)
    if(p->pageaddrs[i])
      count++;
  return count;
}

// --------- Private Methods ------------

void
storesegments(struct dinode *dip, int pageindex, uint *pgaddr)
{
  void *page_segment_addr, *block_addr;
  struct buf *bp;
  int i;

  bp = bread(dev, dip->addrs[NDIRECT]);
  for(i = 0; i < PGSIZE/BSIZE; i++){
    page_segment_addr = pgaddr+(i*BSIZE);
    block_addr = bp->data+(pageindex*PGSIZE/BSIZE)+i;
    memmove(block_addr, page_segment_addr, BSIZE);
    memset(page_segment_addr, 0, BSIZE);
  }
  brelse(bp);
}

void
loadsegments(struct dinode *dip, int pageindex, uint *pgaddr)
{
  void *page_segment_addr, *block_addr;
  struct buf *bp;
  int i;

  bp = bread(dev, dip->addrs[NDIRECT]);
  for(i = 0; i < PGSIZE/BSIZE; i++){ 
    page_segment_addr = pgaddr+(i*BSIZE);
    block_addr = bp->data+(pageindex*PGSIZE/BSIZE)+i;
    memmove(page_segment_addr, block_addr, BSIZE);
    memset(block_addr, 0, BSIZE);
  }
  brelse(bp);
}

uint
gettime(struct proc *p, uint *pageaddr)
{
  int i;
  for(i = 0; i < MAX_PSYC_PAGES; i++)
    if(p->pageaddrs[i] == pageaddr)
      return p->pagetimes[i];
  return -1;
}

void
settime(struct proc *p, uint *pageaddr)
{
  int i;
  for(i = 0; i < MAX_PSYC_PAGES; i++){
    if(!p->pageaddrs[i]){
      p->pageaddrs[i] = pageaddr;
      p->pagetimes[i] = ticks;
      return;
    }
  }
  panic("settime: page not found");
}

void
cleartime(struct proc *p, uint *pageaddr)
{
  int i;
  for(i = 0; i < MAX_PSYC_PAGES; i++){
    if(p->pageaddrs[i] == pageaddr){
      p->pageaddrs[i] = 0;
      p->pagetimes[i] = 0;
      return;
    }
  }
  panic("cleartime: page not found");
}

struct pfile*
pfget(int pid)
{
  struct pfile *pf;
  acquire(&pftable.lock);

  for(pf = pftable.pfiles; pf < &pftable.pfiles[NPROC]; pf++){
    if(pf->pid == pid){
      release(&pftable.lock);
      return pf;
    }
  }

  release(&pftable.lock);
  return (struct pfile*)-1;
}

struct pfile*
pfalloc(int pid)
{
  struct pfile *pf;
  acquire(&pftable.lock);

  for(pf = pftable.pfiles; pf < &pftable.pfiles[NPROC]; pf++){
    if(!pf->pid){
      pf->pid = pid;
      release(&pftable.lock);
      return pf;
    }
  }

  release(&pftable.lock);
  panic("pfalloc: too many procs");
}

void
pffree(struct dinode *dip)
{
  int i;
  struct buf *bp;

  for(i = 0; i < NDIRECT; i++){
    if(dip->addrs[i]){
      bfree(dip->addrs[i]);
      dip->addrs[i] = 0;
    }
  }

  if(dip->addrs[NDIRECT]){
    bp = bread(dev, dip->addrs[NDIRECT]);

    for(i = 0; i < NINDIRECT; i++){
      if(bp->data[i]){
        bfree(bp->data[i]);
        bp->data[i] = 0;
      }
    }

    brelse(bp);
    bfree(dip->addrs[NDIRECT]);
    dip->addrs[NDIRECT] = 0;
  }

  dip->size = 0;
  dip->type = 0;
}

void
bfree(uint b)
{
  struct buf *bp;
  int bi, m;

  bp = bread(dev, BBLOCK(b, sb));
  bi = b % BPB;
  m = 1 << (bi % 8);
  if((bp->data[bi/8] & m) == 0)
    panic("freeing free block");
  bp->data[bi/8] &= ~m;
  brelse(bp);
}

// --------------------------------------
