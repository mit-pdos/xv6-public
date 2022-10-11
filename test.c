#include "types.h"
#include "stat.h"
#include "user.h"

int main(int argc, char *argv[])
{
    fork();
    return exitstat(3);
}long