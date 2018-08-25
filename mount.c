#include "types.h"
#include "defs.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "fs.h"
#include "file.h"
#include "mount.h"

struct {
    struct sleeplock active_mounts_lock; // protects active_mounts
    struct mount_list *active_mounts;
    struct sleeplock mnt_list_lock; // protects mnt_list
    struct mount_list mnt_list[NMOUNT];
} mount_holder;

void mntinit() {
    initsleeplock(&(mount_holder.active_mounts_lock), "active_mounts");
    initsleeplock(&mount_holder.mnt_list_lock, "mount_list");
    // TODO: initialize loop devices
}

void mntget(struct mount *mnt) {
    acquiresleep(&mnt->lock);
    mnt->ref++;
    releasesleep(&mnt->lock);
}

void mntput(struct mount *mnt) {
    acquiresleep(&mnt->lock);
    mnt->ref--;
    releasesleep(&mnt->lock);
}

// mountpoint and device must be locked.
int mount(struct inode *mountpoint, struct inode *device) {
    int i;

    acquiresleep(&mount_holder.active_mounts_lock);
    struct mount_list *current = mount_holder.active_mounts;
    while (current != 0) {
        if (current->mnt.mountpoint == mountpoint) {
            releasesleep(&mount_holder.active_mounts_lock);
            // error - mount on the same mountpoint.
            return -1;
        }
        current = current->next;
    }
    acquiresleep(&mount_holder.mnt_list_lock);
    
    // Find empty mount struct
    for (i = 0; i < NMOUNT && mount_holder.mnt_list[i].mnt.mountpoint != 0; i++);

    if (i == NMOUNT) {
        // error - no available mount memory.
        releasesleep(&mount_holder.mnt_list_lock);
        releasesleep(&mount_holder.active_mounts_lock);
        return -1;
    }

    // TODO: aquire loop device

    struct mount *mnt = &mount_holder.mnt_list[i].mnt;
    mnt->mountpoint = mountpoint;
    mnt->ref = 0;

    // Add to linked list
    mount_holder.mnt_list[i].next = mount_holder.active_mounts;
    mount_holder.active_mounts = &mount_holder.mnt_list[i];

    releasesleep(&mount_holder.mnt_list_lock);
    releasesleep(&mount_holder.active_mounts_lock);

    return 0;
}

int umount(struct inode *mntpoint) {
    acquiresleep(&mount_holder.active_mounts_lock);
    struct mount_list *current = mount_holder.active_mounts;
    struct mount_list **previous = &mount_holder.active_mounts;
    while (current != 0) {
        if (current->mnt.mountpoint == mntpoint) {
            break;
        }
        previous = &current->next;
        current = current->next;
    }

    if (current == 0) {
        releasesleep(&mount_holder.active_mounts_lock);
        // error - not actually mounted.
        cprintf("current=0\n");
        return -1;
    }

    acquiresleep(&current->mnt.lock);
    if (current->mnt.ref > 0) {
        releasesleep(&current->mnt.lock);
        releasesleep(&mount_holder.active_mounts_lock);
        cprintf("current->mnt.ref > 0\n");
        // error - can't unmount as there are references.
        return -1;
    }

    if (current->mnt.parent == 0) {
        // error - can't unmount root filesystem
        releasesleep(&current->mnt.lock);
        releasesleep(&mount_holder.active_mounts_lock);
        cprintf("current->mnt.parent == 0\n");
        return -1;
    }

    acquiresleep(&mount_holder.mnt_list_lock);

    current->mnt.mountpoint = 0;
    mntput(current->mnt.parent);
    //iput(current->mnt.root);
    iput(current->mnt.mountpoint);

    // remove from linked list
    *previous = current->next;

    // TODO: release loop device
    
    releasesleep(&mount_holder.mnt_list_lock);
    releasesleep(&current->mnt.lock);
    releasesleep(&mount_holder.active_mounts_lock);

    return 0;
}

void printmounts() {
    acquiresleep(&mount_holder.active_mounts_lock);

    struct mount_list *entry = mount_holder.active_mounts;
    int i = 0;
    cprintf("Printing mounts:\n");
    while (entry != 0) {
        i++;
        cprintf("Mount %d attached to inode %x\n", i, entry->mnt.mountpoint);
        entry = entry->next;
    }

    releasesleep(&mount_holder.active_mounts_lock);
}