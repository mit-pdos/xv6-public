#include "types.h"
#include "defs.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "fs.h"
#include "file.h"
#include "device.h"

#define NLOOPDEVS (10)
#define NIDEDEVS (2)

struct device {
    struct superblock sb;
    int ref;
    struct inode *ip;
};

struct {
    struct sleeplock lock; // protects mnt_list
    struct device loopdevs[NLOOPDEVS];
    struct superblock idesb[NIDEDEVS];
} dev_holder;

void devinit() {
    initsleeplock(&dev_holder.lock, "dev_list");
}

int getorcreatedevice(struct inode *ip) {
    acquiresleep(&dev_holder.lock);
    int emptydevice = -1;
    for (int i = 0; i < NLOOPDEVS; i++) {
        if (dev_holder.loopdevs[i].ref == 0 && emptydevice == -1) {
            emptydevice = i;        
        } else if (dev_holder.loopdevs[i].ip == ip) {
            dev_holder.loopdevs[i].ref++;
            releasesleep(&dev_holder.lock);
            return i;
        }
    }

    if (emptydevice == -1) {
        releasesleep(&dev_holder.lock);
        return -1;
    }

    dev_holder.loopdevs[emptydevice].ref = 1;
    dev_holder.loopdevs[emptydevice].ip = ip;
    readsb(LOOP_DEVICE_TO_DEV(emptydevice), &dev_holder.loopdevs[emptydevice].sb);
    releasesleep(&dev_holder.lock);
    return LOOP_DEVICE_TO_DEV(emptydevice);
}

void deviceput(uint dev) {
    dev = DEV_TO_LOOP_DEVICE(dev);
    acquiresleep(&dev_holder.lock);
    dev_holder.loopdevs[dev].ref--;

    if (dev_holder.loopdevs[dev].ref == 0) {
        iput(dev_holder.loopdevs[dev].ip);
        dev_holder.loopdevs[dev].ip = 0;
    }
    releasesleep(&dev_holder.lock);
}

struct inode * getinodefordevice(uint dev) {
    if (!IS_LOOP_DEVICE(dev)) {
        return 0;
    }

    dev = DEV_TO_LOOP_DEVICE(dev);

    if (dev_holder.loopdevs[dev].ref == 0) {
        return 0;
    }

    return dev_holder.loopdevs[dev].ip;
}

void printdevices() {
    acquiresleep(&dev_holder.lock);

    cprintf("Printing devices:\n");
    for (int i = 0; i < NLOOPDEVS; i++) {
        if (dev_holder.loopdevs[i].ref != 0) {
            cprintf("Device %d backed by inode %x with ref %d\n", i, dev_holder.loopdevs[i].ip, dev_holder.loopdevs[i].ref);
        }
    }
    releasesleep(&dev_holder.lock);
}

struct superblock * getsuperblock(uint dev) {
    if (IS_LOOP_DEVICE(dev)) {
        dev = LOOP_DEVICE_TO_DEV(dev);
        if (dev >= NLOOPDEVS) {
            return 0;
        }
        if (dev_holder.loopdevs[dev].ref == 0) {
            return 0;
        } else {
            return &dev_holder.loopdevs[dev].sb;
        }
    } else if (dev < NIDEDEVS) {
        return &dev_holder.idesb[dev];
    } else {
        panic("could not find superblock for device");
        return 0;
    }
}