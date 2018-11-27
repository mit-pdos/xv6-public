#include "types.h"
#include "stat.h"
#include "user.h"
#include "uproc.h"
#include "param.h"

static char PROC_STATE[6][10] = {
    "UNUSED",
    "EMBRYO",
    "SLEEPING",
    "RUNNABLE",
    "RUNNING",
    "ZOMBIE",
};

int
main(int argc, char *argv[]) {
    struct uproc up;
    int i;

    printf(1, "pid\tname\tsize\tchannel\tkilled\tstate\n");

    for (i = 0; i < NPROC; i++) {
        if (getprocinfo(i, &up) != 0) {
            continue;
        }
        printf(1, "%d\t%s\t%d\t%d\t%d\t%s\n", up.pid, up.name, up.sz, up.chan, up.killed, PROC_STATE[up.state]);
    }   
    exit();
}
