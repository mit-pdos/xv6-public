struct mount {
    struct mount *parent;
    struct inode *mountpoint;
    struct sleeplock lock; // protects everything below here
    int ref;
    struct loop_device *device;
    struct inode *root;
};

struct mount_list {
    struct mount mnt;
    struct mount_list *next;
};

#define NMOUNT (200)
