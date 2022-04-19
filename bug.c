#include "types.h"
#include "user.h"

int main(int argc, char* argv[])
{
    int val = 1234 / (argc -1);
    printf(1,"%d\n", val);
    exit(0);
}