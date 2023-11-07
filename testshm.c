#include "types.h"
#include "stat.h"
#include "user.h"

int
main(int argc, char *argv[])
{
    char *sharedMemory_p = 0;

    int status = attachSharedMemory(&sharedMemory_p);
    if(0 > status) {
        printf(2, "Error: attachSharedMemory()\n");
    } else {
        printf(1, "PID: %d Attached: %p\n", getpid(), sharedMemory_p);
        
        int counter = 0;
        printf(2, "counter: %p\n", &counter);
        while(counter < 30) {
            if(1 == argc) {
                // no command line argument means read from shared memory
                printf(1, "PID: %d Read: <%s>\n", getpid(), sharedMemory_p);
                sleep(100);
            } else {
                // write message including argv[1]
                snprintf(sharedMemory_p, 4095, 
                    "PID: %d <%d Hello, World!> %s\n", 
                    getpid(), counter, argv[1]);
                sleep(100);
            }
            counter++;
        }
    }
    
    exit();
}
