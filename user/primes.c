#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int primes[30];
int prime_idx = 0;

void solve(int fd) {
    char found_next = 0;
    int pp[2];
    int p, n;

    close(0);
    dup(fd);
    close(fd);

    read(0, &p, sizeof(p));
    // primes[prime_idx++] = p;
    printf("prime %d\n", p);

    while(read(0, &n, sizeof(n))) {
        if(n % p) {
            if(!found_next) {
                found_next = 1;
                pipe(pp);
                if(fork() == 0) { /* Child process */
                    close(pp[1]);
                    solve(pp[0]);
                    exit(0);
                }
                else {
                    close(pp[0]);
                    write(pp[1], &n, sizeof(n));
                }
            }
            else {
                write(pp[1], &n, sizeof(n));
            }
        }
    }
    close(pp[1]);
    wait(0);
    exit(0);
}

int main(int argc, char *argv[])
{
    int pipe1[2];

    pipe(pipe1);
    if(fork() != 0) { /* Parent process */
        close(pipe1[0]);
        for(int i = 2; i <= 35; i++){
            write(pipe1[1], &i, sizeof(i));
        }
        close(pipe1[1]);
        wait(0);
        //for(int i = 0; i < prime_idx; i++) printf("prime %d\n", primes[i]);
    }
    else { /* Child process */
        close(pipe1[1]);
        solve(pipe1[0]);
    }
    exit(0);
}