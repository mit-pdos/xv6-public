#include "types.h"
#include "user.h"

int main(int argc, char *argv[])
{
    int i = 0;
    while (1) {
        printf(1, "seconds: %d\n", i);
        usleep(1 * 1000 * 1000);
        ++i;
    }
    return 0;
}
