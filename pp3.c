#include "types.h"
#include "user.h"
#include "stat.h"

int main(int argc, char **argv)
{
    int pid = 1;
    chpr(pid, 15);
    int i, j;
    for (i = 0; i < 40000; i++)
    {
        for (j = 0; j < 40000; j++)
        {
        }
    }

    exit(0);
}