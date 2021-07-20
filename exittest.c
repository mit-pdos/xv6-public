//test that exits works
#include "types.h"
#include "stat.h"
#include "user.h"
void exittest(void)
{
    int rpid;
    int pid;
    int i;
    int  * status = 0;
    for (i = 0; i < 2; i++)
    {
        pid = fork();
        if (pid == 0)
        {
            if (i == 0)
            {
                printf(1, "\n1 Is child with PID: %d will exit with status %d\n", getpid(), 1);
                exit(1);
            }
            else
            {
                printf(1, "\n2 Is child w/ PID: %d will exit with status %d\n", getpid(), -1);
                exit(-1);
            }
        }
        else if (pid > 0)
        {
            rpid = wait(status);
            printf(1, "Is Parent: child with PID %d has exited w/ status %d\n", rpid, *status);
        }
        else
        {
            printf(1, "ERROR\n");
            exit(-1);
        }
    }
}
/*

void exittest() {
	printf(1, "exittest started\n");
	int pid;
	if ((pid = fork()) == 0) {
		// in child
		
		printf(1, "hello from child\nexiting child with status: %d\n", 1);
		exit(89);
	}
	else {
		// in parent
		printf(1, "hello from parent\n");
		
		int status;
		int childpid = wait(&status);
		
		if (childpid > 0) {
			if (status >= 0) {
				printf(1, "exit status of child pid %d: %d\n", childpid, status);
			}
			else {
				printf(1, "Error in pid %d; no exit status\n", childpid);
				exit(84);
			}
		}
		else {
			printf(1, "error: process had no children\n");
		}
		
		exit(99);
		
	}
}
*/
int main(int argc, char *argv[])

{
    exittest();
    exit(1);
    return 0;
}