#include "types.h"
#include "stat.h"
#include "user.h"

int a = 1;

void test1()
{
    printf(1,"%d free pages before forking\n",getNumFreePages());
    printf(1,"Parent and Child share the global variable a \n");
    int pid = fork();
    if(pid==0)
    {
        printf(1,"Child: a = %d\n",a);
        printf(1,"%d free pages before any changes\n",getNumFreePages());
        a = 2;
        printf(1,"Child: a = %d\n",a);
        printf(1,"%d free pages after changing a\n",getNumFreePages());
        exit();
    }
    printf(1,"Parent: a = %d\n",a);
    wait();
    printf(1,"Parent: a = %d\n",a);
    printf(1,"%d free pages after wait\n",getNumFreePages());
    return ;
}

void test2()
{
    printf(1,"%d free pages before fork-1\n",getNumFreePages());
    if(fork()==0)
    {
        exit();
    }
    else
    {
        printf(1,"%d free pages before fork-2\n",getNumFreePages());
        if(fork()==0)
        {
            printf(1,"%d free pages before changes in Child-2\n",getNumFreePages());
            a = 5;
            printf(1,"%d free pages after changes in Child-2\n",getNumFreePages());
            exit();
        }
        wait();
        printf(1,"%d free pages after reaping Child-1\n",getNumFreePages());
    }
    wait();
    printf(1,"%d free pages after reaping Child-2\n",getNumFreePages());
    return ;
}

void test3()
{
    printf(1,"%d free pages before fork\n",getNumFreePages());
    int pid = fork();
    if(pid==0)
    {
        sleep(4);
        printf(1,"%d free pages before changes in Child\n",getNumFreePages());
        a = 5;
        printf(1,"%d free pages after changes in Child\n",getNumFreePages());
        exit();
    }
    printf(1,"%d free pages before Parent exits\n",getNumFreePages());
    wait();
    return ;
}


int main(void)
{
    printf(1,"====================\n");
    printf(1,"Test1 running\n");
    test1();
    printf(1,"Test1 finished\n");
    printf(1,"====================\n");
    printf(1,"Test2 running\n");
    test2();
    printf(1,"Test2 finished\n");
    printf(1,"====================\n");
    printf(1,"Test3 running\n");
    test3();
    printf(1,"Test3 finished\n");
    printf(1,"====================\n");
    exit();
}
