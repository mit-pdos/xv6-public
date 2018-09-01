#include "types.h"
#include "defs.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "fs.h"
#include "file.h"
#include "device.h"

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
        if (dev_holder.devices[i].ref == 0 && emptydevice == -1) {
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
    readsb(LOOP_DEVICE_TO_DEV(emptydevice), &dev_holder.devices[emptydevice].sb);
    releasesleep(&dev_holder.lock);
    return LOOP_DEVICE_TO_DEV(emptydevice);
}

void deviceput(int dev) {
    dev = DEV_TO_LOOP_DEVICE(dev);
    acquiresleep(&dev_holder.lock);
    dev_holder.devices[dev].ref--;

    if (dev_holder.devices[dev].ref == 0) {
        iput(dev_holder.devices[dev].ip);
        dev_holder.devices[dev].ip = 0;
    }
    releasesleep(&dev_holder.lock);
}

struct inode * getinodefordevice(int dev) {
    if (!IS_LOOP_DEVICE(dev)) {
        return 0;
    }

    dev = DEV_TO_LOOP_DEVICE(dev);

    if (dev_holder.devices[dev].ref == 0) {
        return 0;
    }

    return dev_holder.devices[dev].ip;
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