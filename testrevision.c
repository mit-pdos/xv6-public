#include "types.h"
#include "user.h"

int main(void)
{
    // fork into parent and child; parent will kill child after a short sleep
    int pid = fork();
    if (!pid) // child
    {
        while (1)
        {
            printf(1, "I am the child, my pid is %d\n", getpid());
            sleep(300);
        }
    }
    sleep(10);
    printf(1, "I am the parent, pid %d\n", getpid());
    // parent code
    sleep(1000);
    printf(1, "parent killing child %d...", pid);
    kill(pid);
    sleep(10);
    printf(1, "done.\n");
}
