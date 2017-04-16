// @TODO: Properly hard-coded dev
// @TODO: reuse existing superblock
// @TODO: named files accessible in user mode
// @TODO: write page to file
// @TODO: read page from file
//      - 8 blocks / page
//      - 12 blocks direct
//      - 128 blocks indirect
//      - 8*15 = 120 blocks needed
// @TODO: Use loaduvm, allocuvm, deallocuvm
// @TODO: Figure out how to swap data + stack pages
// @TODO: Load directly to file on exec

#include "types.h"
#include "defs.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "param.h"
#include "mmu.h"
#include "fs.h"
#include "buf.h"
#include "file.h"

void bfree(int dev, uint b);
void pffree(struct dinode *dip);
struct pfile* pfalloc(int pid);
struct pfile* pfget(int pid);

struct pfile {
  struct dinode * dip;
  int pid;
};

struct {
  struct spinlock lock;
  struct pfile pfiles[NPROC];
} pftable;

struct superblock sb;
int dev = 1;

// --------- Public Methods ------------

void
pfcopy(int parentpid, int pid)
{
  int inum;
  struct buf *bp;
  struct dinode *dip;
  struct pfile *pf, *parentpf;

  for(inum = 1; inum < sb.ninodes; inum++) {
    bp = bread(dev, IBLOCK(inum, sb));
    dip = (struct dinode*)bp->data + inum%IPB;

    if(dip->type == 0) {
      parentpf = pfget(parentpid);
      memmove(dip, parentpf->dip, sizeof(*dip));
      brelse(bp);
      pf = pfalloc(pid);
      pf->dip = dip;
      return;
    }

    brelse(bp);
  }
  panic("pfcreate: no inodes");
}

void
storepage(int pid, void* pgaddr)
{
  struct pfile *pf;
  pf = pfget(pid);

}

void
loadpage(int pid, void* pgaddr)
{
  struct pfile *pf;
  pf = pfget(pid);

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

// --------- Private Methods ------------

struct pfile*
pfget(int pid)
{
  struct pfile *pf;

  acquire(&pftable.lock);
  for(pf = pftable.pfiles; pf < &pftable.pfiles[NPROC]; pf++){
    if(pf->pid){
      release(&pftable.lock);
      return pf;
    }
  }
  release(&pftable.lock);
  panic("pfget: pfile not found");
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
  uint *a;

  if(dip->nlink == 0){
    for(i = 0; i < NDIRECT; i++){
      if(dip->addrs[i]){
        bfree(dev, dip->addrs[i]);
        dip->addrs[i] = 0;
      }
    }

    if(dip->addrs[NDIRECT]){
      bp = bread(dev, dip->addrs[NDIRECT]);
      a = (uint*)bp->data;
      for(i = 0; i < NINDIRECT; i++){
        if(a[i])
          bfree(dev, a[i]);
      }
      brelse(bp);
      bfree(dev, dip->addrs[NDIRECT]);
      dip->addrs[NDIRECT] = 0;
    }

    dip->size = 0;
    dip->type = 0;
  }
}

void
bfree(int dev, uint b)
{
  struct buf *bp;
  struct buf *bp1;
  int bi, m;

  bp1 = bread(dev, 1);
  memmove(&sb, bp1->data, sizeof(sb));
  brelse(bp1);

  bp = bread(dev, BBLOCK(b, sb));
  bi = b % BPB;
  m = 1 << (bi % 8);
  if((bp->data[bi/8] & m) == 0)
    panic("freeing free block");
  bp->data[bi/8] &= ~m;
  brelse(bp);
}

// --------------------------------------
