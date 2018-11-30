#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

int
main(int argc, char* argv[])
{
    printf(1,"[success] realloc.c main test!\n");
    realloc();
    // urealloc(atoi(argv[argc-1]), 1024*3); // demo less size
    urealloc(atoi(argv[argc-1]), 4096*2); // demo bigger size
    exit();
}
