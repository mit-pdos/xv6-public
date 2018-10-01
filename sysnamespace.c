#include "types.h"
#include "defs.h"
#include "param.h"
#include "stat.h"
#include "mmu.h"
#include "proc.h"
#include "param.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "fs.h"
#include "file.h"
#include "mount.h"

int
sys_unshare(void) {
    int nstype;
    if (argint(0, &nstype) < 0) {
        return -1;
    }

    return unshare(nstype);
}