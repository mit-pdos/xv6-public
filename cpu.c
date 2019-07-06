#include "types.h"
#include "user.h"

int main(int argc, char *argv[])
{
    int microseconds = 0;

    if (argc != 2) {
        printf(2, "usage: %s [sleep_microseconds]\n", argv[0]);
        exit(1);
    }

    microseconds = atoi(argv[1]);

    while (1) {
        printf(1, "cpu time: %d, cpu percent: %d\n",
               ioctl(-1, IOCTL_GET_PROCESS_CPU_TIME),
               ioctl(-1, IOCTL_GET_PROCESS_CPU_PERCENT));
        if (microseconds) {
            usleep(microseconds);
        }
    }
    return 0;
}
