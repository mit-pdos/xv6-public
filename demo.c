#include "param.h"
#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "fcntl.h"
#include "syscall.h"
#include "traps.h"
#include "memlayout.h"

char buf[512];

int main(int argc, char *argv[])
{
    int pid;

    pid = fork();

    if (pid == 0)
    {

        // printf(1,"123\n");
        // printf(1,"456\n");
        int fd = 0, st;
        for (int i = 1; i < argc; i++)
        {
            if ((fd = open(argv[i], 0)) < 0)
            {
                printf(1, "Cant open");
                return 0;
            }
        }
        int request = 1;
        flockwr(fd, &st, request);
        int n;
        int a = write(fd, "123", 4);
        if(a == -1)
            printf(1, "write fail up\n");
        while ((n = read(fd, buf, sizeof(buf))) > 0)
        {
            for (int i = 0; i < sizeof(buf); i++)
            {
                printf(1, "%c", buf[i]);
                wait();
            }
        }
        close(fd);
    }
    else if (pid > 0)
    {
        int fd = 0, st;
        for (int i = 1; i < argc; i++)
        {
            if ((fd = open(argv[i], 0)) < 0)
            {
                printf(1, "Cant open");
                return 0;
            }
        }
        int request = 1;
        flockwr(fd, &st, request);
        int n;
        int a = write(fd, "123", 4);
        if(a == -1)
            printf(1, "write fail down\n");
        while ((n = read(fd, buf, sizeof(buf))) > 0)
        {
            for (int i = 0; i < sizeof(buf); i++)
            {
                printf(1, "%c", buf[i]);
            }
        }
        wait();
    }
    else
    {
        printf(1, "fork() failed\n");
        exit();
    }
}
