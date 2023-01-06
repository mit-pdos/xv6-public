#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    int pp1[2], pp2[2];
    char c;

    pipe(pp1);
    pipe(pp2);

    if(fork() == 0) { /* Child process */
        close(0);
        dup(pp1[0]);
        close(pp1[0]);

        if(read(0, &c, 1)) {
            printf("%d: received ping\n", getpid());
            write(pp2[1], "a", 1);
            exit(0);
        }

    }
    else { /* Parent process */
        close(0);
        dup(pp2[0]);
        close(pp2[0]);

        write(pp1[1], "a", 1);
        close(pp1[1]);

        if(read(0, &c, 1)) {
            printf("%d: received pong\n", getpid());
            exit(0);
        }
        
    }
    exit(0);
}
