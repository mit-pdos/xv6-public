#include "types.h"
#include "user.h"
#include "fcntl.h"
int main(int argc, char *argv[]){
    char buffer[512];
    // Make sure the usage is correct by first checking for correct number of perameters
    if(argc > 3 || argc < 3){
        printf(1, "Not the correct usage of command cp");
        exit();
        //OR CALL man cp
    }
    //check if source and destination can open
    int srcf = open(argv[1], O_RDONLY); //src file
    int destf = open(argv[2], O_CREATE|O_RDWR); // dest file
    if(srcf < 0){
        printf(1, "Cannot open source file: %s \n", argv[1]);
        exit();
    }
    if (destf < 0){
        printf(1, "Cannot open / Read or write destination file: %s \n ", argv[2]);
        exit();
    }
    int i;
    while((i = read(srcf, buffer, sizeof(buffer))) > 0){
        write(destf, buffer, i);
    }
    close(srcf);
    close(destf);
    exit();


}