struct mount {
    struct mount *parent;
    struct inode *mountpoint;
    int ref;
    uint dev;
    struct inode *root;
};

struct mount_list {
    struct mount mnt;
    struct mount_list *next;
};

#define NMOUNT (200)
