#include "types.h"
#include "stat.h"
#include "user.h"

static volatile int counter = 0;

int loop(int n)
{
    for(int i = n * 4096; i > 0; --i) {
        for(int j = n * 4096; j > 0; --j) {
            counter += 1;
        }
    }
    return counter;
}

int
main(int argc, char *argv[])
{
    for(int j = 3; j > 0; --j) {
        loop(j);
    }
    exit();
}
