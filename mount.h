struct mount {
    struct mount *parent;
    struct inode *mountpoint;
    int ref;
    uint dev;
    struct inode *root;
};

#define NMOUNT (200)
