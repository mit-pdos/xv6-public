#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

int main(int argc, char* argv[]) {
    if (fork()) {
        printf(1, "%s", "Parent.\n");
        int retime = 44;
        int rutime = 43;
        int stime = 42;
        printf(1, "ADDRESSES @Userland\nRetime=%d\nRutime=%d\nStime=%d\n", &retime, &rutime, &stime);
        wait2(&retime, &rutime, &stime);

        printf(1, "Values after wait2\nRetime=%d\nRutime=%d\nStime=%d\n", retime, rutime, stime);
    } else {
        printf(1, "%s", "Child.\n");
        int i;
        int x;
        for (i = 0; i < 10000; i++) {
            x =  x + 3.14 * 109.32 / 3;
        }
    }
    exit();
}
