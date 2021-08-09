#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

int main(int argc, char *argv[])
{
    int childPid;
    int priority1, priority2;
    priority1 = 19;
    priority2 = 15;
    int z, x;
    childPid = fork();
    int pid2 = fork();
    int pid3 = fork();
    if (childPid == 0)
    { //in child process
        printf(1, "Child: Changed process %d's priority to %d\n", chpr(getpid() - 1, priority1), priority1);
    }
    else //in parent process
    {
        printf(1, "Parent: Changed process %d's priority to %d\nn", chpr(childPid, priority2), priority2);
    }
    if (pid2 == 0)
    printf(1,"In child2\n" );
    if (pid3 == 0)
    printf(1,"In child3\n" );
    
    for (z = 0; z < 12u; z += 1)
    {
        //cps();
        //printf(1, "Process %d with priority %d is running!\n", m;)
        x = x + 3.14 * 89.64; //Useless calculation to consume CPU Time
    }
    cps();
    
    printf(1, "About to wait\n");
    wait(0);
    wait(0);
    wait(0);
    wait(0);
    wait(0);
    printf(1, "Exiting process %d\n", getpid());
    exit(0);
    return 0;
}