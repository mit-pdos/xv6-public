#include "types.h"
#include "stat.h"
#include "user.h"

#define N 10
#define M 1000

int
main(void) {
//    int w_time[N];
//    int r_time[N];
//    int io_time[N];
//    int PID[N];
//    int w_time_tmp, r_time_tmp, io_time_tmp;

    int i;
    int j;
    int pid;

    for (i = 0; i < N; i++) {
        if ((pid = fork()) < 0)
            exit();
        if (pid == 0) {
            for (j = 0; j < M; j++)
                printf(1, "child %d prints for the %d time\n", getpid(), j);
            printf(1, "\n");
            exit();
        }
    }
//    for (i = 0; i < N; i++) {
//        PID[i] = wait2(&w_time_tmp, &r_time_tmp, &io_time_tmp);
//        w_time[i] = w_time_tmp;
//        io_time[i] = io_time_tmp;
//        r_time[i] = r_time_tmp;
//    }
//
//    for (i = 0; i < N; i++) {
//        printf(1, "child %d: w_time - %u, r_time - %u, io_timew - %u\n", PID[i], w_time[i], r_time[i], io_time[i]);
//    }
    exit();
}
