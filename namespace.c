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

struct {
  struct spinlock lock;
  struct nsproxy nsproxy[NNAMESPACE];
} namespacetable;

void namespaceinit() {
    initlock(&namespacetable.lock, "namespace");
}

void namespaceput(struct nsproxy* nsproxy) {
    acquire(&namespacetable.lock);
    nsproxy->ref--;
    release(&namespacetable.lock);
}

struct nsproxy* namespacedup(struct nsproxy* nsproxy) {
    acquire(&namespacetable.lock);
    nsproxy->ref++;
    release(&namespacetable.lock);
    return nsproxy;
}

static struct nsproxy* allocnsproxyinternal(void) {
    for (int i = 0; i < NNAMESPACE; i++) {
        if (namespacetable.nsproxy[i].ref == 0) {
            namespacetable.nsproxy[i].ref++;
            return &namespacetable.nsproxy[i];
        }
    }

    panic("out of nsproxy objects");
}

struct nsproxy* allocnsproxy(void) {
    acquire(&namespacetable.lock);
    struct nsproxy* result = allocnsproxyinternal();
    release(&namespacetable.lock);

    return result;
}

int unshare(int nstype) {
    acquire(&namespacetable.lock);
    if (myproc()->nsproxy->ref > 1) {
        myproc()->nsproxy = allocnsproxyinternal();
        // copy all nsproxy fields
    }
    release(&namespacetable.lock);
    switch(nstype) {
        default:
            return -1;
    }
}