//test that exits works
//expected output is 5
#include "types.h"
#include "stat.h"
#include "user.h"

void exittest(void)
{
    exit(5);
}

int main(void)
{
    exittest();
    printf("%s\n", "hello");
}