#include "types.h"
#include "stat.h"
#include "user.h"

int main(int argc, char *argv[]) {

    countTraps();// return 2 for sbrk and exec

    if (fork() == 0) { // +1
        exit(); //+ 1
    }
    wait(); //+ 1

    countTraps(); // return 5 system calls
    countTraps(); // return 6 system calls

    // 2 * 11 = 22 system calls
    for(int i = 0; i <= 10; i++) {
        if (fork() == 0) {
            exit();
        }
        wait();
    }

    countTraps(); // 29 system calls

    exit();
}