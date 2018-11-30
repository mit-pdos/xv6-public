#include "types.h"
#include "user.h"

int main(void)
{
	int parent = getpid();
    int pid = fork();

    if (pid == 0)
    {
         printf(1, "This is child: Pid = %d, parent = %d, Use getpid() = %d, Use getppid() = %d.\n",
         	         pid, parent, getpid(), getppid());
    }

    else if (pid > 0)
    {
        wait();
    	printf(1, "This is parent: Pid = %d, parent = %d, Use getpid() = %d, Use getppid() = %d.\n",
         	         pid, parent, getpid(), getppid());
    }

    exit();
}