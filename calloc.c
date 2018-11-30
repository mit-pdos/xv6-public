#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

int
main(int argc, char* argv[])
{
    printf(1,"successfully called calloc_");
    calloc_(atoi(argv[argc-1]),4096);
    exit();
}
