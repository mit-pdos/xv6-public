//
// File-system system calls.
// Mostly argument checking, since we don't trust
// user code, and calls into file.c and fs.c.
//

#include "types.h"
#include "defs.h"
#include "param.h"
#include "stat.h"
#include "mmu.h"
#include "proc.h"
#include "fs.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "file.h"
#include "mount.h"

int sys_mount() {
    char *device_path;
    char *mount_path;
    struct inode *device, *mount_dir;
    struct mount *parent;
    cprintf("sys_mount\n");
    if (argstr(0, &device_path) < 0 || argstr(1, &mount_path) < 0) {
        cprintf("badargs\n");
        return -1;
    }

    cprintf("begin_op\n");
    begin_op();
    cprintf("after begin_op\n");

    if ((device = namei(device_path)) == 0) {
        cprintf("bad device_path\n");
        end_op();
        return -1;
    }

    if ((mount_dir = nameimount(mount_path, &parent)) == 0) {
        cprintf("bad mount_path\n");
        iput(device);
        end_op();
        return -1;
    }

    if (mount_dir->inum == ROOTINO) {
        cprintf("can't mount root directory of another mount.\n");
        iput(device);
        iput(mount_dir);
        mntput(parent);
        end_op();
        return -1;
    }

    cprintf("locking\n");
    ilock(device);
    ilock(mount_dir);

    cprintf("after locking\n");
    if (mount_dir->type != T_DIR) {
        cprintf("badinodetype\n");
        iunlockput(device);
        iunlockput(mount_dir);
        mntput(parent);
        end_op();
        return -1;
    }

    cprintf("calling!\n");
    int res = mount(mount_dir, device, parent);

    iunlock(mount_dir);
    if (res != 0) {
        iput(mount_dir);
    }

    iunlockput(device);
    mntput(parent);
    end_op();

    return res;
}

int sys_umount() {
    char *mount_path;
    struct inode *mount_dir;
    struct mount *mnt;
    if (argstr(0, &mount_path) < 0) {
        cprintf("badargs\n");
        return -1;
    }

    begin_op();

    if ((mount_dir = nameimount(mount_path, &mnt)) == 0) {
        cprintf("badinodes\n");
        end_op();
        return -1;
    }

    iput(mount_dir);

    cprintf("calling!\n");    
    int res = umount(mnt);
    if (res != 0) {
        mntput(mnt);
    }
    end_op();
    return res;
}

int sys_printmounts() {
    printmounts();
    return 0;
}

int sys_printdevices() {
    printdevices();
    return 0;
}