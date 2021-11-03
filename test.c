#include "param.h"
#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "fcntl.h"
#include "syscall.h"
#include "traps.h"
#include "memlayout.h"


#define NUM 5
int stdout = 1;

int main()
{

    
    for (int i=0 ; i<NUM ; i++)
    {
        int pid = fork();
        if (pid == 0)
        {
            //child
            printf(stdout, "Child (pid : %d) created\n", getpid());
            for (int i = 0 ; i < 2000000000 ; i++)
            {
                sleep(10);
            }   

            exit();
        }
        else
        {
            //parent
            printf(stdout, "Parent (pid : %d) creating child %d (pid = %d)\n", getpid(), i, pid);


        }

    }

    for (int i=0 ; i < NUM ; i++)
    {
        int killed_child_pid = wait();
        printf(stdout, "Child (pid : %d) killed\n", killed_child_pid);
    }

}







