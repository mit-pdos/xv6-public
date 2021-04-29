#include "kernel/types.h"
#include "kernel/stat.h"
#include "user.h"

int main(int argc, char *argv[]){
    // int i = 0;
    
    // while(i < 5000){
    //     i++;
    //     //fprintf(1,"%d\n",i);
    // }
    // i = 5; 
    if(strcmp(argv[1],"-now")==0){
        cpuhalt();
    }
    printf("Shutting Down in 5 seconds\n");
    if(fork()==0){
        sleep(500);

        cpuhalt();
    }
    else{
        exit();
    }
    
}