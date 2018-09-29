struct mount_ns {
    int ref;
    struct spinlock lock; // protects active_mounts
    struct mount_list *active_mounts;
};