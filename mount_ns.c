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
#include "mount_ns.h"

struct {
  struct spinlock lock;
  struct mount_ns mount_ns[NNAMESPACE];
} mountnstable;

void mount_nsinit() {
    initlock(&mountnstable.lock, "mountns");
}

struct mount_ns* mount_nsdup(struct mount_ns* mount_ns) {
    acquire(&mountnstable.lock);
    mount_ns->ref++;
    release(&mountnstable.lock);

    return mount_ns;
}

void mount_nsput(struct mount_ns* mount_ns) {
    acquire(&mountnstable.lock);
    mount_ns->ref--;
    release(&mountnstable.lock);
}