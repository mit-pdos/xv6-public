#include "user.h"
#include "types.h"
#include "stat.h"

int main(void){

    int procesos = getprocs();

    printf("N° de procesos en CPU: %d", procesos);

    exit();}


