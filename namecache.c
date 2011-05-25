//
// file-system name cache
//
// to do:
//   use ns.c namespaces
//   maybe generation #s don't need to be in on-disk inode
//     if namecache only referred to cached inodes
//   insert when file created, not just looked up
//   eviction
//   verify that it hits when it is supposed to
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
  uint gen;   // child ip->gen
};
#define NCE 128
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
  uint gen = 0;

  acquire(&nc_lock);
  struct nce *e = nc_lookup1(dir, name);
  if(e){
    inum = e->cinum;
    gen = e->gen;
  }
  release(&nc_lock);

  if(inum){
    struct inode *ip = iget(dir->dev, inum);
    // this is a bit ugly
    // maybe there should be a real inode cache and
    // the namecache should contain direct refs.
    int ok = (ip->gen == gen);
    if(ok){
      return ip;
    } else {
      iput(ip);
      return 0;
    }
  } else {
    return 0;
  }
}

// dir is locked, but ip is not.
void
nc_insert(struct inode *dir, char *name, struct inode *ip)
{
  // though ip is not locked, it's still OK to 
  // look at ip->gen.

  acquire(&nc_lock);

  struct nce *e = nc_lookup1(dir, name);
  if(e){
    if(e->cinum != ip->inum || e->gen != ip->gen){
      // someone forgot to call nc_invalidate()
      panic("nc_insert change");
    }
    release(&nc_lock);
    return;
  }

  int i;
  for(i = 0; i < NCE; i++){
    e = &nce[i];
    if(e->valid == 0){
      e->valid = 1;
      e->dev = dir->dev;
      e->dinum = dir->inum;
      strncpy(e->name, name, DIRSIZ);
      e->cinum = ip->inum;
      e->gen = ip->gen;
      break;
    }
  }
  if(i >= NCE)
    cprintf("namecache full\n");

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
