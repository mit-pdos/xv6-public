#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

int
main(int argc, char const *argv[])
{
    printf(1,"los procesos en ejecucion son %d\n ",getprocs());
    exit();
}
