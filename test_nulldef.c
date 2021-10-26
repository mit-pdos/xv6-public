#include "types.h"
#include "user.h"
#include "fnmatch.h"
#include "stat.h"

int stdout = 1 ;
int stderr = 2; 

int main(int argc, char* argv[]){
    int *p =(int *) atoi(argv[1]);
    printf(stdout, "Hello world %d\n", *p);
    exit();
}