#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"

int
main(int argc, char *argv[])
{
    int ticks;
    if(argc != 2) {
        printf(1, "%s\n", "Error: sleep requires exactly 1 argument");
        exit();
    }
    ticks = atoi(argv[1]);
    if(ticks <= 0) {
        printf(1, "%s\n", "Error: sleep ticks must be a positive integer");
        exit();
    }
    sleep(ticks);
    //printf(1, "Ticks ran: %d\n", ticks_running(getpid()));
    exit();
}