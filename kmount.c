#include "types.h"
#include "defs.h"
#include "spinlock.h"
#include "fs.h"
#include "sleeplock.h"
#include "file.h"
#include "mount.h"
#include "param.h"
#include "stat.h"
#include "mmu.h"
#include "proc.h"
#include "namespace.h"
#include "mount_ns.h"

struct mount_list {
  struct mount mnt;
  struct mount_list *next;
};

struct {
  struct spinlock mnt_list_lock; // protects mnt_list
  struct mount_list mnt_list[NMOUNT];
} mount_holder;

struct mount_list*
getactivemounts()
{
  return myproc()->nsproxy->mount_ns->active_mounts;
}

// Parent mount (if it exists) must already be ref-incremented.
static void
addmountinternal(struct mount_list *mnt_list, uint dev, struct vfs_inode *mountpoint, struct mount *parent) {
    mnt_list->mnt.mountpoint = mountpoint;
    mnt_list->mnt.dev = dev;
    mnt_list->mnt.parent = parent;

    // add to linked list
    mnt_list->next = getactivemounts();
    myproc()->nsproxy->mount_ns->active_mounts = mnt_list;
}

struct mount *
getinitialrootmount(void)
{
  return &mount_holder.mnt_list[0].mnt;
}

struct mount *
getrootmount(void)
{
  return myproc()->nsproxy->mount_ns->root;
}

void
mntinit(void)
{
  initlock(&mount_holder.mnt_list_lock, "mount_list");

  addmountinternal(&mount_holder.mnt_list[0], ROOTDEV, 0, 0);
  mount_holder.mnt_list[0].mnt.ref = 1;
  myproc()->nsproxy->mount_ns->root = getinitialrootmount();
}

struct mount*
mntdup(struct mount *mnt)
{
  acquire(&mount_holder.mnt_list_lock);
  mnt->ref++;
  release(&mount_holder.mnt_list_lock);
  return mnt;
}

void
mntput(struct mount *mnt)
{
  acquire(&mount_holder.mnt_list_lock);
  mnt->ref--;
  release(&mount_holder.mnt_list_lock);
}

static struct mount_list*
allocmntlist(void)
{
  acquire(&mount_holder.mnt_list_lock);
  int i;
  // Find empty mount struct
  for (i = 0; i < NMOUNT && mount_holder.mnt_list[i].mnt.ref != 0; i++);

  if (i == NMOUNT) {
    // error - no available mount memory.
    panic("out of mount_list objects");
  }

  struct mount_list *newmountentry = &mount_holder.mnt_list[i];
  newmountentry->mnt.ref = 1;

  release(&mount_holder.mnt_list_lock);

  return newmountentry;
}

// mountpoint and device must be locked.
int
mount(struct vfs_inode *mountpoint, struct vfs_inode *device, struct mount *parent) {
    struct mount_list *newmountentry = allocmntlist();
    struct mount *newmount = &newmountentry->mnt;

    int dev = getorcreatedevice(device);
    if (dev < 0) {
        newmount->ref = 0;
        cprintf("failed to create device.\n");
        return -1;
    }

    acquire(&myproc()->nsproxy->mount_ns->lock);
    struct mount_list *current = getactivemounts();
    while (current != 0) {
        if (current->mnt.parent == parent && current->mnt.mountpoint == mountpoint) {
            // error - mount already exists.
            release(&myproc()->nsproxy->mount_ns->lock);
            deviceput(dev);
            newmount->ref = 0;
            cprintf("mount already exists at that point.\n");
            return -1;
        }
        current = current->next;
    }

    mntdup(parent);
    addmountinternal(newmountentry, dev, mountpoint, parent);

    release(&myproc()->nsproxy->mount_ns->lock);
    return 0;
}

int
objfs_mount(struct vfs_inode *mountpoint, struct vfs_inode *device, struct mount *parent) {
    struct mount_list *newmountentry = allocmntlist();
    struct mount *newmount = &newmountentry->mnt;

    int dev = getorcreateobjdevice();
    if (dev < 0) {
        newmount->ref = 0;
        cprintf("failed to create device.\n");
        return -1;
    }

    acquire(&myproc()->nsproxy->mount_ns->lock);
    struct mount_list *current = getactivemounts();
    while (current != 0) {
        if (current->mnt.parent == parent && current->mnt.mountpoint == mountpoint) {
            // error - mount already exists.
            release(&myproc()->nsproxy->mount_ns->lock);
            deviceput(dev);
            newmount->ref = 0;
            cprintf("mount already exists at that point.\n");
            return -1;
        }
        current = current->next;
    }

    mntdup(parent);
    addmountinternal(newmountentry, dev, mountpoint, parent);
    release(&myproc()->nsproxy->mount_ns->lock);
    return 0;
}

int
umount(struct mount *mnt)
{
  acquire(&myproc()->nsproxy->mount_ns->lock);
  struct mount_list *current = getactivemounts();
  struct mount_list **previous = &(myproc()->nsproxy->mount_ns->active_mounts);
  while (current != 0) {
    if (&current->mnt == mnt) {
      break;
    }
    previous = &current->next;
    current = current->next;
  }

  if (current == 0) {
    // error - not actually mounted.
    release(&myproc()->nsproxy->mount_ns->lock);
    cprintf("current=0\n");
    return -1;
  }

  if (current->mnt.parent == 0) {
    // error - can't unmount root filesystem
    release(&myproc()->nsproxy->mount_ns->lock);
    cprintf("current->mnt.parent == 0\n");
    return -1;
  }

  acquire(&mount_holder.mnt_list_lock);
  
  // Base ref is 1, +1 for the mount being acquired before entering this method.
  if (current->mnt.ref > 2) {
    // error - can't unmount as there are references.
    release(&mount_holder.mnt_list_lock);
    release(&myproc()->nsproxy->mount_ns->lock);
    return -1;
  }

  // remove from linked list
  *previous = current->next;
  release(&myproc()->nsproxy->mount_ns->lock);

  struct vfs_inode *oldmountpoint = current->mnt.mountpoint;
  int olddev = current->mnt.dev;
  current->mnt.mountpoint = 0;
  current->mnt.parent->ref--;
  current->mnt.ref = 0;
  current->mnt.dev = 0;
  current->next = 0;
  
  release(&mount_holder.mnt_list_lock);

    oldmountpoint->i_op.iput(oldmountpoint);
    deviceput(olddev);
    return 0;
}

struct mount *
mntlookup(struct vfs_inode *mountpoint, struct mount *parent) {
    acquire(&myproc()->nsproxy->mount_ns->lock);

  struct mount_list *entry = getactivemounts();
  while (entry != 0) {
    if (entry->mnt.mountpoint == mountpoint && entry->mnt.parent == parent) {
      release(&myproc()->nsproxy->mount_ns->lock);
      return mntdup(&entry->mnt);
    }
    entry = entry->next;
  }

  release(&myproc()->nsproxy->mount_ns->lock);
  return 0;
}

void
printmounts(void)
{
  acquire(&myproc()->nsproxy->mount_ns->lock);

  struct mount_list *entry = getactivemounts();
  int i = 0;
  cprintf("Printing mounts:\n");
  while (entry != 0) {
    i++;
    cprintf("%d: Mount %x attached to %x, child of %x, with ref %d\n", i, &entry->mnt, entry->mnt.mountpoint, entry->mnt.parent, entry->mnt.ref);
    entry = entry->next;
  }

  release(&myproc()->nsproxy->mount_ns->lock);
}

void
umountall(struct mount_list* mounts)
{
  while (mounts != 0) {
    struct mount_list* next = mounts->next;
    if (mounts->mnt.parent == 0) {
      // No need to unmount root - 
      mounts->mnt.ref = 0;
    } else if (umount(&mounts->mnt) != 0) {
      panic("failed to umount upon namespace close");
    }
    mounts = next;
  }
}

static struct mount_list *
shallowcopyactivemounts(struct mount **newcwdmount) {
    struct mount_list *head = 0;
    struct mount_list *entry = myproc()->nsproxy->mount_ns->active_mounts;
    struct mount_list *prev = 0;
    while (entry != 0) {
        struct mount_list *newentry = allocmntlist();
        if (head == 0) {
            head = newentry;
        }
        newentry->mnt.ref = 1;
        newentry->mnt.mountpoint = entry->mnt.mountpoint->i_op.idup(entry->mnt.mountpoint);
        newentry->mnt.parent = 0;
        newentry->mnt.dev = entry->mnt.dev;
        deviceget(newentry->mnt.dev);
        if (prev != 0) {
            prev->next = newentry;
        }

    if (myproc()->cwdmount == &entry->mnt) {
      *newcwdmount = &newentry->mnt;
    }

    prev = newentry;
    entry = entry->next;
  }
  

  return head;
}

static void
fixparents(struct mount_list* newentry)
{
  struct mount_list *entry = myproc()->nsproxy->mount_ns->active_mounts;

  while (entry != 0) {    
    if (entry->mnt.parent != 0) {
      struct mount_list *finder = myproc()->nsproxy->mount_ns->active_mounts;
      struct mount_list *newfinder = newentry;
      while (finder != 0 && entry->mnt.parent != &finder->mnt) {
        finder = finder->next;
        newfinder = newfinder->next;
      }

      if (finder == 0) {
        panic("invalid mount tree structure");
      }

      newentry->mnt.parent = mntdup(&newfinder->mnt);
    }

    newentry = newentry->next;
    entry = entry->next;
  }
}

struct mount_list*
copyactivemounts(void)
{
  acquire(&myproc()->nsproxy->mount_ns->lock);
  struct mount *oldcwdmount = myproc()->cwdmount;
  struct mount *newcwdmount = 0;
  struct mount_list *newentry = shallowcopyactivemounts(&newcwdmount);
  fixparents(newentry);
  release(&myproc()->nsproxy->mount_ns->lock);

  myproc()->cwdmount = mntdup(newcwdmount);
  mntput(oldcwdmount);
  return newentry;
}

struct mount*
getroot(struct mount_list* newentry)
{
  if (newentry != 0) {
    struct mount* current = &newentry->mnt;
    while (current != 0 && current->parent != 0) {
      current = current->parent;
    }

    if (current == 0) {
      panic("malformed mount structure - no root");
    }

    return current;
  }

  return 0;
}
