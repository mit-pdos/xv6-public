
#include "types.h"
#include "user.h"

#define NULL ((void *) 0)
int
main() {
    int *i = malloc(4);
    int j = 5;
    printf(1, "int *i at heap address %p\n", i);
    printf(1, "int j = %d at stack address %p\n", j, &j);
    printf(1, "Setting int* i to point to memory address of int j (%p)\n", &j);
    i = &j;
    printf(1, "*i = %d at address %p\n", *i, i);
    printf(1, "Setting int* i value to 10\n");
    *i = 10;
    printf(1, "*i = %d at address %p\n", *i, i);
    printf(1, "j = %d at address %p\n", j, &j);
    i = NULL;
    printf(1, "*i = %d at address %p\n", *i, i);

    exit();
}