#include "types.h"
#include "defs.h"
#include "spinlock.h"
#include "fs.h"
#include "sleeplock.h"
#include "file.h"
#include "mount.h"
#include "param.h"
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

struct nsproxy* allocnsproxy(void) {
    acquire(&namespacetable.lock);
    for (int i = 0; i < NNAMESPACE; i++) {
        if (namespacetable.nsproxy[i].ref == 0) {
            namespacetable.nsproxy[i].ref++;
            release(&namespacetable.lock);
            return &namespacetable.nsproxy[i];
        }
    }
    release(&namespacetable.lock);

    panic("out of nsproxy objects");
}