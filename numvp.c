#include "types.h"
#include "stat.h"
#include "user.h"

int
main(void) {
    printf(1, "return val of system call is %d\n", numvp());
    exit();
}
