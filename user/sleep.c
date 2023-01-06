#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    if(argc == 1) {
        printf("ERROR: need more argument\n");
        exit(1);
    }
    sleep(atoi(argv[1]));   
    exit(0);
}
