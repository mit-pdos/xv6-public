#include "types.h"
#include "stat.h"
#include "fcntl.h"
#include "user.h"

int main(int argc, char *argv[]) {

    int fd = open(argv[1], O_RDONLY);
    if(fd < 0) {
        write(2, "Cannot open ", 13);
        write(2, argv[1], strlen(argv[1]));
        write(2, "\n", 2);
    }

    char arr[1024];
    int n = read(fd, arr, 1024);
    if(n < 0) {
        write(2, "Read Error\n", 12);
        close(fd);
    }

    int totLine = 0; //cnt total no. of lines
    for(int i = 0; i < n; i++) {
        if(arr[i] == '\n') {
            totLine++;
        }
    }

    int skipLine = totLine - 10; //we require only last 10 lines
    if(skipLine < 0) skipLine = 0;

    int curr = 0; //for skip starts lines, print only last 10 lines
    for(int i = 0; i < n; i++) {
        if(curr >= skipLine) {
            write(1, &arr[i], 1);
        }
        if(arr[i] == '\n') {
            curr++;
        }
    }

    close(fd);
    exit();
}