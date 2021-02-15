#include "types.h"
#include "user.h"




int main(int argc, char *argv[]){
    if(argc == 1){
        printf(2, "Hello, user!\n");
    }
    else if(argc > 3){
        printf(1, "Too many arguments!\n");
    }
    else{
        printf(1, "Hello, %s \n", argv[1]);
    }
    exit();
}