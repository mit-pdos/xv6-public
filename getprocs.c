#include "user.h"
#include "types.h"
#include "stat.h"

// Dice cuantos son los procesos siendo ejecutados en CPU

int main(void){

    int procesos = getprocs();

    printf("N° de procesos en CPU: %d", procesos);

    exit();}


