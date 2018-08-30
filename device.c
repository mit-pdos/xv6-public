#include "types.h"
#include "defs.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "fs.h"
#include "file.h"

#define NDEVICES (10)

struct device {
    struct superblock sb;
    int ref;
    struct inode *ip;
};

struct {
    struct sleeplock lock; // protects mnt_list
    struct device devices[NDEVICES];
} dev_holder;

void devinit() {
    initsleeplock(&dev_holder.lock, "dev_list");
}

int getorcreatedevice(struct inode *ip) {
    acquiresleep(&dev_holder.lock);
    int emptydevice = -1;
    for (int i = 0; i < NDEVICES; i++) {
        if (dev_holder.devices[i].ref == 0) {
            emptydevice = i;        
        } else if (dev_holder.devices[i].ip == ip) {
            dev_holder.devices[i].ref++;
            releasesleep(&dev_holder.lock);
            return i;
        }
    }

    if (emptydevice == -1) {
        releasesleep(&dev_holder.lock);
        return -1;
    }

    dev_holder.devices[emptydevice].ref = 1;
    dev_holder.devices[emptydevice].ip = ip;
    // TODO: initialize superblock
    releasesleep(&dev_holder.lock);
    return emptydevice;
}

void deviceput(int dev) {
    acquiresleep(&dev_holder.lock);
    dev_holder.devices[dev].ref--;

    if (dev_holder.devices[dev].ref == 0) {
        iput(dev_holder.devices[dev].ip);
    }
    releasesleep(&dev_holder.lock);
}

void printdevices() {
    acquiresleep(&dev_holder.lock);

    cprintf("Printing devices:\n");
    for (int i = 0; i < NDEVICES; i++) {
        if (dev_holder.devices[i].ref != 0) {
            cprintf("Device %d backed by inode %x with ref %d\n", i, dev_holder.devices[i].ip, dev_holder.devices[i].ref);
        }
    }
    releasesleep(&dev_holder.lock);
}