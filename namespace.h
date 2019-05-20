struct nsproxy {
    int ref;
    struct mount_ns* mount_ns;
    struct pid_ns* pid_ns;
};
