#include <stdio.h>
#include <stdlib.h>
void panic(const char* msg) {
    printf("kernel panic: %s\n", msg);
    exit(1);
}
