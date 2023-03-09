#include "types.h"
#include "stat.h"
#include "user.h"


int
main(int argc, char *argv[])
{
    if (argc == 1 || argc > 2) {
        printf(1, "error: expected 1 arg, recieved %d\n", argc - 1);
        exit();
    }
    int n = atoi(argv[1]);
    printf(1, "mmap returned %d\n", mmap(n));
    exit();
}