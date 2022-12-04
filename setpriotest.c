#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

int main(int argc, char* argv[]) {
    int i;
    for (i = -1; i < 4; ++i){
        int val = set_prio(i);
        printf(1, "Value returned by set_prio with argument %d: %d\n", i, val);
    }
    exit();
}