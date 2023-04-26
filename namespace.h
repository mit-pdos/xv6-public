#ifndef XV6_NAMESPACE_H
#define XV6_NAMESPACE_H
struct nsproxy {
    int ref;
    struct mount_ns* mount_ns;
    struct pid_ns* pid_ns;
};

struct nsproxy* namespace_replace_pid_ns(struct nsproxy* oldns, struct pid_ns* pid_ns);

#endif /* XV6_NAMESPACE_H */
 