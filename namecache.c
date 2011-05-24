//
// file-system name cache
//
// to do:
//   use ns.c namespaces
//   invalidation (for rename, unlink)
//   does directory inum need to be locked?
//   need a lock to make table lookup and iget atomic?
//   insert when file created, not just looked up
//

#include "types.h"
#include "defs.h"
#include "param.h"
#include "stat.h"
#include "mmu.h"
#include "spinlock.h"
#include "condvar.h"
#include "queue.h"
#include "proc.h"
#include "buf.h"
#include "fs.h"
#include "file.h"

struct spinlock nc_lock;

// map the tuple <dev,dinum,name> to cinum;
struct nce {
  int valid;
  uint dev;
  uint dinum; // directory inumber
  char name[DIRSIZ];
  uint cinum; // child inumber
};
#define NCE 32
struct nce nce[NCE];

void
nc_init()
{
  initlock(&nc_lock, "namecache");
}

// nc_lock must be held
struct nce *
nc_lookup1(struct inode *dir, char *name)
{
  for(int i = 0; i < NCE; i++){
    struct nce *e = &nce[i];
    if(e->valid && e->dev == dir->dev && e->dinum == dir->inum &&
       namecmp(name, e->name) == 0){
      return e;
    }
  }
  return 0;
}

struct inode *
nc_lookup(struct inode *dir, char *name)
{
  uint inum = 0;

  acquire(&nc_lock);
  struct nce *e = nc_lookup1(dir, name);
  if(e)
    inum = e->cinum;
  release(&nc_lock);

  if(inum)
    return iget(dir->dev, inum);
  else
    return 0;
}

void
nc_insert(struct inode *dir, char *name, struct inode *ip)
{
  acquire(&nc_lock);

  struct nce *e = nc_lookup1(dir, name);
  if(e){
    if(e->cinum != ip->inum)
      panic("nc_insert change");
    release(&nc_lock);
    return;
  }

  for(int i = 0; i < NCE; i++){
    e = &nce[i];
    if(e->valid == 0){
      e->valid = 1;
      e->dev = dir->dev;
      e->dinum = dir->inum;
      strncpy(e->name, name, DIRSIZ);
      e->cinum = ip->inum;
      break;
    }
  }

  release(&nc_lock);
}

void
nc_invalidate(struct inode *dir, char *name)
{
  acquire(&nc_lock);
  struct nce *e = nc_lookup1(dir, name);
  if(e)
    e->valid = 0;
  release(&nc_lock);
}
