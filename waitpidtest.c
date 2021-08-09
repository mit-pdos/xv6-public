#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

int waitpidtest(void)
{
    int i;
    int *status = 0;
    int mypid;
    int pid[3] = {0, 0, 0};

    for (i = 0; i < 3; i++)
    {
        pid[i] = fork();
        if (pid[i] == 0)
            exit(0);
    }
    sleep(5);
    printf(1, "In parent\nWaiting for child %d\n", pid[2]);
    mypid = waitpid(pid[2], status, 0);
    printf(1, "child %d has exited\n", mypid);
    sleep(5);
    printf(1, "In parent\nWaiting for child %d\n", pid[0]);
    mypid = waitpid(pid[0], status, 0);
    printf(1, "child %d has exited\n", mypid);
    sleep(5);
    printf(1, "In parent\nWaiting for child %d\n", pid[1]);
    mypid = waitpid(pid[1], status, 0);
    printf(1, "child %d has exited\n", mypid);
    return 0;
}
int main(int argc, char *argv[])

{
    waitpidtest();
    exit(1);
    return 0;
}