#include "types.h"
#include "stat.h"
#include "user.h"

int main(int argc, const char *argv[]) {
    if(1 >= argc) {
        write(2, "Usage: <path>\n", 14);
    } else {
        for(int i = 1; i < argc; i++) {
            const char *path = argv[i];
            int fd = open(path, 0);
            if(0 > fd) {
                write(2, "Error Opening to file:\n", 23);
            } else {
                char buffer[2] = {0,0};
                int status = 0;
                do {
                    status = read(fd, buffer, 1);
                    write(1, buffer, 1);
                } while(0 < status);

                if(0 > status) {
                    write(2, "Error: Writing to file:\n", 25);
                }
            }
            close(fd);
        }
    }
    return 0;
}
