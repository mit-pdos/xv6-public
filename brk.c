#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

int
main(int argc, char* argv[])
{
    brk_de(atoi(argv[argc-1]));
    exit();
}
