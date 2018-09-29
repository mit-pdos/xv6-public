struct nsproxy {
    int ref;
    struct mount_ns* mount_ns;
};
