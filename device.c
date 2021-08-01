#include "types.h"
#include "defs.h"
#include "spinlock.h"
#include "fs.h"
#include "sleeplock.h"
#include "file.h"
#include "device.h"
#include "obj_fs.h"
#include "obj_disk.h"

#define NLOOPDEVS (10)
#define NIDEDEVS (2)
#define NOBJDEVS (2)

struct device {
  struct superblock sb;
  int ref;
  struct inode *ip;
};

struct obj_device {
    struct objsuperblock sb;
    int ref;
};

struct {
  struct spinlock lock; // protects loopdevs
  struct device loopdevs[NLOOPDEVS];
  struct superblock idesb[NIDEDEVS];
  struct obj_device objdev[NOBJDEVS];
} dev_holder;

void
devinit(void)
{
  initlock(&dev_holder.lock, "dev_list");
}

void
objdevinit(void)
{
    memcpy(&dev_holder.objdev[0].sb , memory_storage, sizeof(dev_holder.objdev[0].sb));
}

int
getobjdevice()
{
    objdevinit();
    // TODO: This is a hard coded device number for obj device
    return OBJ_TO_DEV(0);
}

int
getorcreatedevice(struct inode *ip)
{
  acquire(&dev_holder.lock);
  int emptydevice = -1;
  for (int i = 0; i < NLOOPDEVS; i++) {
    if (dev_holder.loopdevs[i].ref == 0 && emptydevice == -1) {
      emptydevice = i;
    } else if (dev_holder.loopdevs[i].ip == ip) {
      dev_holder.loopdevs[i].ref++;
      release(&dev_holder.lock);
      return LOOP_DEVICE_TO_DEV(i);
    }
  }

  if (emptydevice == -1) {
    release(&dev_holder.lock);
    return -1;
  }

  dev_holder.loopdevs[emptydevice].ref = 1;
  dev_holder.loopdevs[emptydevice].ip = idup(ip);
  release(&dev_holder.lock);
  fsinit(LOOP_DEVICE_TO_DEV(emptydevice));
  return LOOP_DEVICE_TO_DEV(emptydevice);
}

void deviceget(uint dev)
{
  if (IS_LOOP_DEVICE(dev)) {
    dev = DEV_TO_LOOP_DEVICE(dev);
    acquire(&dev_holder.lock);
    dev_holder.loopdevs[dev].ref++;
    release(&dev_holder.lock);
  }
}

void
deviceput(uint dev)
{
  if (IS_LOOP_DEVICE(dev)) {
    dev = DEV_TO_LOOP_DEVICE(dev);
    acquire(&dev_holder.lock);
    if (dev_holder.loopdevs[dev].ref == 1) {
      release(&dev_holder.lock);

      iput(dev_holder.loopdevs[dev].ip);
      invalidateblocks(LOOP_DEVICE_TO_DEV(dev));

      acquire(&dev_holder.lock);
      dev_holder.loopdevs[dev].ip = 0;
    }
    dev_holder.loopdevs[dev].ref--;
    release(&dev_holder.lock);
  } else if (IS_OBJ_DEVICE(dev)) {
      dev = DEV_TO_OBJ_DEVICE(dev);
      acquire(&dev_holder.lock);
      // TODO: Add functional for obj device
      release(&dev_holder.lock);
  }
}

struct inode*
getinodefordevice(uint dev)
{
  if (!IS_LOOP_DEVICE(dev)) {
    return 0;
  }

  dev = DEV_TO_LOOP_DEVICE(dev);

  if (dev_holder.loopdevs[dev].ref == 0) {
    return 0;
  }

  return dev_holder.loopdevs[dev].ip;
}

void
printdevices(void)
{
  acquire(&dev_holder.lock);

  cprintf("Printing devices:\n");
  for (int i = 0; i < NLOOPDEVS; i++) {
    if (dev_holder.loopdevs[i].ref != 0) {
      cprintf("Device %d backed by inode %x with ref %d\n", i, dev_holder.loopdevs[i].ip, dev_holder.loopdevs[i].ref);
    }
  }

  for (int i = 0; i < NOBJDEVS; i++) {
      // TODO: add ref count to OBJDEV.
//      if (dev_holder.objdev[i].ref != 0) {
          cprintf("Device %d with storage_device_size of superblock %d\n", i, dev_holder.objdev[i].sb.storage_device_size);
//      }
  }
  release(&dev_holder.lock);
}

struct superblock*
getsuperblock(uint dev)
{
  if (IS_LOOP_DEVICE(dev)) {
    uint loopdev = DEV_TO_LOOP_DEVICE(dev);
    if (loopdev >= NLOOPDEVS) {
      panic("could not find superblock for device: device number to high");
    }
    if (dev_holder.loopdevs[loopdev].ref == 0)
    {
      panic("could not find superblock for device: device ref count is 0");
    }
    else
    {
      return &dev_holder.loopdevs[loopdev].sb;
    }
  } else if (dev < NIDEDEVS) {
    return &dev_holder.idesb[dev];
  } else {
    panic("could not find superblock for device");
  }
}

// TODO: not useful for now, maybe to remove it later if not needed
struct objsuperblock*
getobjsuperblock(uint dev)
{
    if (IS_OBJ_DEVICE(dev)) {
        uint objdev = DEV_TO_OBJ_DEVICE(dev);
        if (objdev >= NOBJDEVS) {
            panic("could not find superblock for device: device number to high");
        }
        return &dev_holder.objdev[objdev].sb;
    } else {
        panic("could not find superblock for device - not an obj device");
    }
}

int
doesbackdevice(struct inode* ip)
{
  acquire(&dev_holder.lock);
  for (int i = 0; i < NLOOPDEVS; i++) {
    if (dev_holder.loopdevs[i].ip == ip) {
      release(&dev_holder.lock);
      return 1;
    }
  }
  release(&dev_holder.lock);
  return 0;
}