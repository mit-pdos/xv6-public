#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

int
main(int argc, char* argv[])
{
    calloc_(atoi(argv[argc-1]),4096);
    exit();
}
