//
// File-system system calls.
// Mostly argument checking, since we don't trust
// user code, and calls into vfs_file.c and vfs_fs.c.
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
#include "cgroup.h"


int
sys_mount(void)
{
    char *fstype;
    char *device_path;
    char *mount_path;
    struct mount *parent;

    if (argstr(2, &fstype) < 0) {
        cprintf("badargs\n");
        return -1;
    }

    // Mount objfs file system
    if(strcmp(fstype, "objfs") == 0) {
        struct vfs_inode *mount_dir;
        if (argstr(1, &mount_path) < 0) {
            cprintf("badargs\n");
            return -1;
        }

        begin_op();

        if ((mount_dir = vfs_nameimount(mount_path, &parent)) == 0) {
            end_op();
            return -1;
        }

        mount_dir->i_op.ilock(mount_dir);

        int res = objfs_mount(mount_dir, 0, parent);
        mount_dir->i_op.iunlock(mount_dir);
        if (res != 0) {
            mount_dir->i_op.iput(mount_dir);
        }

        mntput(parent);
        end_op();

        return res;
    } else if(strcmp(fstype, "cgroup") == 0) {
        struct vfs_inode *mount_dir;
        if (argstr(0, &device_path) < 0 || argstr(1, &mount_path) < 0 || device_path != 0) {
            cprintf("badargs\n");
            return -1;
        }

        begin_op();

        if ((mount_dir = vfs_nameimount(mount_path, &parent)) == 0) {
            cprintf("bad mount_path\n");
            end_op();
            return -1;
        }

        if(*(cgroup_root()->cgroup_dir_path)){
            cprintf("cgroup filesystem already mounted\n");
            end_op();
            return -1;
        }

        set_cgroup_dir_path(cgroup_root(), mount_path);
        objfs_mount(mount_dir, 0, parent);
        mntput(parent);
        end_op();

        return 0;

    } else {

        struct vfs_inode *device, *mount_dir;
        if (argstr(0, &device_path) < 0 || argstr(1, &mount_path) < 0) {
            cprintf("badargs\n");
            return -1;
        }

        begin_op();

        if ((device = vfs_namei(device_path)) == 0) {
            cprintf("bad device_path\n");
            end_op();
            return -1;
        }

        if ((mount_dir = vfs_nameimount(mount_path, &parent)) == 0) {
            device->i_op.iput(device);
            end_op();
            return -1;
        }

        if (mount_dir->inum == ROOTINO) {
            device->i_op.iput(device);
            mount_dir->i_op.iput(mount_dir);
            mntput(parent);
            end_op();
            return -1;
        }

        device->i_op.ilock(device);
        mount_dir->i_op.ilock(mount_dir);

        if (mount_dir->type != T_DIR) {
            device->i_op.iunlockput(device);
            mount_dir->i_op.iunlockput(mount_dir);
            mntput(parent);
            end_op();
            return -1;
        }

        int res = mount(mount_dir, device, parent);

        mount_dir->i_op.iunlock(mount_dir);
        if (res != 0) {
            mount_dir->i_op.iput(mount_dir);
        }

        device->i_op.iunlockput(device);
        mntput(parent);
        end_op();

        return res;
    }
}

int
sys_umount(void)
{
    char *mount_path;
     if (argstr(0, &mount_path) < 0) {
        cprintf("badargs\n");
        return -1;
    }

    begin_op();

    int delete_cgroup_res = cgroup_delete(mount_path, "umount");

    if(delete_cgroup_res == -1){
        struct vfs_inode *mount_dir;
        struct mount *mnt;

        if ((mount_dir = vfs_nameimount(mount_path, &mnt)) == 0) {
            end_op();
            return -1;
        }

        if (mount_dir->inum != ROOTINO) {
            mount_dir->i_op.iput(mount_dir);
            mntput(mnt);
            end_op();
            return -1;
        }

        mount_dir->i_op.iput(mount_dir);

        int res = umount(mnt);
        if (res != 0) {
            mntput(mnt);
        }

        end_op();
        return res;
    }

    if(delete_cgroup_res == -2){
        end_op();
        cprintf("cannot unmount cgroup\n");
        return -1;
    }

    end_op();
    return delete_cgroup_res;
}

int
sys_printmounts(void)
{
    printmounts();
    return 0;
}

int
sys_printdevices(void)
{
    printdevices();
    return 0;
}
