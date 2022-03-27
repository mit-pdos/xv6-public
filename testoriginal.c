#include "types.h"
#include "user.h"

int main(void)
{
    // fork into parent and child; parent will kill child after a short sleep
    int pid = fork();
    if (!pid) // child
    {
        printf(1, "I am the child, my pid is %d\n", getpid());
        while (1)
        {
            sleep(30);
            printf(1, "child heartbeat <3\n");
        }
    }
    sleep(10);
    printf(1, "I am the parent, my pid is %d\n", getpid());
    // parent code
    sleep(100);
    printf(1, "parent killing child pid=%d...", pid);
    kill(pid,0);
    sleep(10);
    printf(1, "done.\nexiting parent.\n");

    exit();
}
