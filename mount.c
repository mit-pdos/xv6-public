#include "user.h"

int main(int argc, const char * const argv[])
{
    const char usage[] =
        "Usage:\nmount [-t [fstype]] [path]\nmount [path] [-t [fstype]]\n";
    const char * fstype = 0;
    const char * path = 0;
    int index = 0;

    for (index = 1; index < argc;) {
        if (!strcmp(argv[index], "-t")) {
            ++index;
            if (index >= argc || fstype) {
                printf(2, usage);
                exit(1);
            }
            fstype = argv[index];
            ++index;
            continue;
        }

        if (path) {
            printf(2, usage);
            exit(1);
        }

        path = argv[index];
        ++index;
    }

    if (!path) {
        printf(2, usage);
        exit(1);
    }

    exit(mount(0, path, fstype));
}
