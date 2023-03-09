#include "types.h"
#include "stat.h"
#include "user.h"


int
main(int argc, char *argv[])
{
    int n = atoi(argv[1]);
    printf(1, "mmap returned %d\n", mmap(n));
    exit();
}