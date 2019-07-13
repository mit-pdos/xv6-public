#include "user.h"

int main(int argc, const char * const argv[])
{
    const char usage[] = "Usage: umount [path]\n";

    if (argc != 2) {
        printf(2, usage);
        exit(1);
    }

    exit(umount(argv[1]));
}
