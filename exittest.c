//test that exits works
#include "types.h"
#include "stat.h"
#include "user.h"

void exittest(void)
{
    int pid;
    int i;
    int status;
    for (i = 0; i < 2; i++)
    {
        pid = 0;
        pid = fork();
        if (pid < 0)
            break;
        if (pid == 0)
        {
            printf(1, "\nIs child with PID %d\n", getpid());
            exit(5);
        }
        else
        {
            int cpid = wait(&status);
            printf(1, "\nIs Parent and child w/ PID: %d exited with status %d\n", cpid, status);
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