#include "types.h"
#include "stat.h"
#include "fcntl.h"
#include "user.h"

int main(int argc, char *argv[]) {

    int fd = open(argv[1], O_RDONLY);
    if(fd < 0) {
        write(2, "Cannot Open: ", 13);
        write(2, argv[1], strlen(argv[1]));
        write(2, "\n", 2);
    }

    char arr;
    int lines = 0;
    int n;
    while((n = read(fd, &arr, 1) > 0)) {
        write(1, &arr, 1);

        //cnt no. of lines
        if(arr == '\n') {
            lines++;
        }
        //exit after lines == 10
        if(lines == 10) break;
    }

    close(fd);
}