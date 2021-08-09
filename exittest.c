//test that exits works
#include "types.h"
#include "stat.h"
#include "user.h"
void exittest(void)
{
    int rpid;
    int pid;
    int i;
    int *status = 0;
    for (i = 0; i < 2; i++)
    {
        pid = fork();
        if (pid == 0)
        {
            if (i == 0)
            {
                printf(1, "\n1 In child with PID: %d will exit with status %d\n", getpid(), 1);
                exit(1);
            }
            else
            {
                printf(1, "\n2 In child w/ PID: %d will exit with status %d\n", getpid(), -1);
                exit(-1);
            }
        }
        else if (pid > 0)
        {
            rpid = wait(status);
            printf(1, "In Parent, child with PID %d has exited w/ status %d\n", rpid, *status);
        }
        else
        {
            printf(1, "ERROR\n");
            exit(-1);
        }
    }
}
int main(int argc, char *argv[])

{
    exittest();
    exit(1);
    return 0;
}