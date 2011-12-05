//
// file-system name cache
//
// to do:
//   eviction
//   maybe generation #s don't need to be in on-disk inode
//     if namecache only referred to cached inodes
//

#include "types.h"
#include "defs.h"
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
struct ns *ncns;

// map the tuple <dev,dinum,name> to cinum;
struct nce {
  uint cinum; // child inumber
  uint gen;   // child ip->gen
};

void
nc_init()
{
  ncns = nsalloc(0);
  initlock(&nc_lock, "namecache");
}

struct inode *
nc_lookup(struct inode *dir, char *name)
{
  rcu_begin_read();
  struct nce *e = ns_lookup(ncns, KIIS(dir->dev, dir->inum, name));
  if (!e)
    return 0;

  struct inode *ip = iget(dir->dev, e->cinum);
  // this is a bit ugly
  // maybe there should be a real inode cache and
  // the namecache should contain direct refs.
  int ok = (ip->gen == e->gen);
  rcu_end_read();
  if(ok){
    return ip;
  } else {
    iput(ip);
    return 0;
  }
}

// dir is locked, but ip is not.
void
nc_insert(struct inode *dir, char *name, struct inode *ip)
{
  // though ip is not locked, it's still OK to 
  // look at ip->gen.

  rcu_begin_read();
  struct nce *e = ns_lookup(ncns, KIIS(dir->dev, dir->inum, name));
  if(e){
    if(e->cinum != ip->inum || e->gen != ip->gen){
      // someone forgot to call nc_invalidate()
      panic("nc_insert change");
    }
    rcu_end_read();
    return;
  }
  rcu_end_read();

  e = kmalloc(sizeof(*e));
  e->cinum = ip->inum;
  e->gen = ip->gen;
  ns_insert(ncns, KIIS(dir->dev, dir->inum, name), e);
}

void
nc_invalidate(struct inode *dir, char *name)
{
  ns_remove(ncns, KIIS(dir->dev, dir->inum, name), 0);
}
