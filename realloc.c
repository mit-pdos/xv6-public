#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

int
main(int argc, char* argv[])
{
    printf(1,"[success] realloc.c main test!\n");
    realloc();
    void* p = malloc(4096);
    // urealloc(atoi(argv[argc-1]), 1024*3); // demo less size
    urealloc(p, 4096*2); // demo bigger size
    exit();
}
