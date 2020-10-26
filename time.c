#include "types.h"
#include "user.h"
#include "stat.h"
#include "fs.h"

int main(int argc, char *argv[]){
    if(argc < 2){
        printf(1, "Unsufficient arguments supplied\n");
    }
    else{
        int childPid = fork();
        if(childPid == 0){
            char * args[argc-1];
            for(int i=1; i<argc; i++){
                args[i-1] = argv[i];
            }
            exec(args[0], args);
            printf(1, "Error, %s cannot be executed\n", argv[1]);
            exit();
        }
        else{
            int wtime =0, rtime =0;
            if(waitx(&wtime, &rtime) != -1){
                printf(1, "For %s :\n", argv[1]);
                printf(1, "> Wait Time = %d\n", wtime);
                printf(1, "> Run Time = %d\n", rtime);
            }
            else{
                printf(1, "Error in waitx\n");
            }
            exit();
        }
    }
}