#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

int
main(int argc, char *argv[])
{
   int fd = open("lock", O_CREATE|O_RDWR);
   if(0 <= fd) {
    printf(2, "Trying to get lock\n");
    int status = flock(fd);
    if(0 > status) {
        printf(2, "Error: flock()\n");
    } else {
        printf(2, "Got lock\n");
        sleep(500);
        printf(2, "Unlocking\n");
        funlock(fd);
    }
    }
   exit();
}
