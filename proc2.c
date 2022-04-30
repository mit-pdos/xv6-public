#include "types.h"
#include "stat.h"
#include "user.h"

int main(int argc, char *argv[]){
    setprior(15);
    int i, k;
    for (i = 0; i < 43000; i++){
        asm("nop");
        for (k=0; k < 43000; k++){
            asm("nop");
        }
    }
    exit();
}