#include "kernel/types.h"
#include "user/user.h"




int main(int argc, char *argv[]){
    if(argc == 1){
        printf("Hello, user!\n");
    }
    else if(argc > 3){
        printf("Too many arguments!\n");
    }
    else{
        printf("Hello, %s \n", argv[1]);
    }
    exit();
}