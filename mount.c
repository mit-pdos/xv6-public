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

struct mount_list* getactivemounts() {
    return myproc()->nsproxy->mount_ns->active_mounts;
}

// Parent mount (if it exists) must already be ref-incremented.
void addmountinternal(struct mount_list *mnt_list, uint dev, struct inode *mountpoint, struct mount *parent) {
    mnt_list->mnt.mountpoint = mountpoint;
    mnt_list->mnt.dev = dev;
    mnt_list->mnt.parent = parent;

    // add to linked list
    mnt_list->next = getactivemounts();
    myproc()->nsproxy->mount_ns->active_mounts = mnt_list;
}

struct mount * getrootmount() {
    return &mount_holder.mnt_list[0].mnt;
}

void mntinit() {
    initlock(&mount_holder.mnt_list_lock, "mount_list");

    addmountinternal(&mount_holder.mnt_list[0], ROOTDEV, 0, 0);
    mount_holder.mnt_list[0].mnt.ref = 1;
}

struct mount * mntdup(struct mount *mnt) {
    acquire(&mount_holder.mnt_list_lock);
    mnt->ref++;
    release(&mount_holder.mnt_list_lock);
    return mnt;
}

void mntput(struct mount *mnt) {
    acquire(&mount_holder.mnt_list_lock);
    mnt->ref--;
    release(&mount_holder.mnt_list_lock);
}

// mountpoint and device must be locked.
int mount(struct inode *mountpoint, struct inode *device, struct mount *parent) {
    acquire(&mount_holder.mnt_list_lock);
    int i;
    // Find empty mount struct
    for (i = 0; i < NMOUNT && mount_holder.mnt_list[i].mnt.ref != 0; i++);

    if (i == NMOUNT) {
        // error - no available mount memory.
        release(&mount_holder.mnt_list_lock);
        cprintf("no free mount entry\n");
        return -1;
    }

    struct mount_list *newmountentry = &mount_holder.mnt_list[i];
    struct mount *newmount = &newmountentry->mnt;

    newmount->ref = 1;

    release(&mount_holder.mnt_list_lock);

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
        } else if (current->mnt.root == mountpoint) {
            // error - mount on the root of another mound.
            release(&myproc()->nsproxy->mount_ns->lock);
            deviceput(dev);
            newmount->ref = 0;
            cprintf("can't mount to root of another mount\n");
            return -1;
        }
        current = current->next;
    }

    mntdup(parent);

    addmountinternal(newmountentry, dev, mountpoint, parent);
    release(&myproc()->nsproxy->mount_ns->lock);
    return 0;
}

int umount(struct mount *mnt) {
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

    struct inode *oldmountpoint = current->mnt.mountpoint;
    int olddev = current->mnt.dev;
    current->mnt.mountpoint = 0;
    current->mnt.parent->ref--;
    current->mnt.ref = 0;
    current->mnt.dev = 0;
    
    release(&mount_holder.mnt_list_lock);

    iput(oldmountpoint);
    deviceput(olddev);
    return 0;
}

struct mount * mntlookup(struct inode *mountpoint, struct mount *parent) {
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

void printmounts() {
    acquire(&myproc()->nsproxy->mount_ns->lock);

    struct mount_list *entry = getactivemounts();
    int i = 0;
    cprintf("Printing mounts:\n");
    while (entry != 0) {
        i++;
        cprintf("Mount %d attached to inode %x with ref %d\n", i, entry->mnt.mountpoint, entry->mnt.ref);
        entry = entry->next;
    }

    release(&myproc()->nsproxy->mount_ns->lock);
}

void umountall(struct mount_list* mounts) {
    while (mounts != 0) {
        struct mount_list* next = mounts->next;
        if (umount(&mounts->mnt) != 0) {
            panic("failed to umount upon namespace close");
        }
        mounts = next;
    }
}