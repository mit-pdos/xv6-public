#include "types.h"
#include "stat.h"
#include "user.h"

#define N 10
#define M 1000

void
printChild() {
    int i;
    int pid = getpid();
    for (i = 0; i < M; i++)
        printf(1, "child %d prints for the %d time\n", pid, i);
}

int
main(void) {
    int w_time[N];
    int r_time[N];
    int s_time[N];
    int PID[N];
    int w_time_tmp, r_time_tmp, s_time_tmp;

    int i;
    int pid;

    for (i = 0; i < N; i++) {
        if ((pid = fork()) < 0)
            exit();
        if (pid == 0) {
            printChild();
            printf(1, "\n");
            exit();
        }
    }
    for (i = 0; i < N; i++) {
        PID[i] = wait2(&w_time_tmp, &r_time_tmp, &s_time_tmp);
        w_time[i] = w_time_tmp;
        s_time[i] = s_time_tmp;
        r_time[i] = r_time_tmp;
    }

    printf(1, "Waiting, running, sleeping, total\n");
    for (i = 0; i < N; i++) {
        printf(1, "child %d: w_time - %d, r_time - %d, s_time - %d, total time: %d\n",
               PID[i], w_time[i], r_time[i], s_time[i],
               w_time[i] + r_time[i] + s_time[i]);
    }
    exit();
}
