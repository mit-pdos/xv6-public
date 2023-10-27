#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

static const char* message = 
    "If you see this string then writing to dev/hello works.\n";
static char s_buffer[4000];

int main(int argc, const char *argv[]) {
    printf(2, "starting <%s>\n", argv[0]);
    int fd = open("dev/hello", O_RDWR);
    printf(2, "opened dev/hello\n");

    int pid = fork();
    if(0 == pid) {
        printf(2, "child\n");
        sleep(500);
        printf(2, "child writing: %d\n", fd);
        write(fd, message, strlen(message));
    } else {
        memset(s_buffer, 0, 4000);
        printf(2, "parent reading: %d\n", fd);
        read(fd, s_buffer, 3999);
        printf(2, "%s\n", s_buffer);
        wait();
    }
    close(fd);
    fd = -1;
 
    exit();
    return 0;
}
