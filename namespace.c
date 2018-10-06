#include "types.h"
#include "defs.h"
#include "param.h"
#include "stat.h"
#include "mmu.h"
#include "proc.h"
#include "spinlock.h"
#include "fs.h"
#include "sleeplock.h"
#include "file.h"
#include "mount.h"
#include "namespace.h"
#include "ns_types.h"

struct {
  struct spinlock lock;
  struct nsproxy nsproxy[NNAMESPACE];
} namespacetable;

void
namespaceinit(void)
{
    initlock(&namespacetable.lock, "namespace");
    mount_nsinit();
}

void
namespaceput(struct nsproxy* nsproxy)
{
    acquire(&namespacetable.lock);
    if (nsproxy->ref == 1) {
        release(&namespacetable.lock);
        mount_nsput(nsproxy->mount_ns);
        nsproxy->mount_ns = 0;
        acquire(&namespacetable.lock);
    }
    nsproxy->ref -= 1;
    release(&namespacetable.lock);
}

struct nsproxy*
namespacedup(struct nsproxy* nsproxy)
{
    acquire(&namespacetable.lock);
    nsproxy->ref++;
    release(&namespacetable.lock);
    return nsproxy;
}

static struct nsproxy*
allocnsproxyinternal(void)
{
    for (int i = 0; i < NNAMESPACE; i++) {
        if (namespacetable.nsproxy[i].ref == 0) {
            namespacetable.nsproxy[i].ref++;
            return &namespacetable.nsproxy[i];
        }
    }

    panic("out of nsproxy objects");
}

struct nsproxy*
emptynsproxy(void)
{
    acquire(&namespacetable.lock);
    struct nsproxy* result = allocnsproxyinternal();
    result->mount_ns = newmount_ns();
    release(&namespacetable.lock);

    return result;
}

int
unshare(int nstype)
{
    acquire(&namespacetable.lock);
    if (myproc()->nsproxy->ref > 1) {
        struct nsproxy *oldns = myproc()->nsproxy;
        myproc()->nsproxy = allocnsproxyinternal();
        myproc()->nsproxy->mount_ns = mount_nsdup(oldns->mount_ns);
        oldns->ref--;
    }
    release(&namespacetable.lock);
    switch(nstype) {
        case MOUNT_NS:
            {
                struct mount_ns* previous = myproc()->nsproxy->mount_ns;
                myproc()->nsproxy->mount_ns = copymount_ns();
                mount_nsput(previous);
                return 0;
            }
        default:
            return -1;
    }
}