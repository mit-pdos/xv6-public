#include "stat.h"
#include "types.h"
#include "user.h"

int main(int argc, char *argv[]) {
    void *shp = malloc(512);
    printf("%p\n", shp);
    exit();
}

int main(int argc, char *argv[]) {
    // char *testPage = GetSharedPage(8, 64);
    // exit();

    // char *testPage = GetSharedPage(4, 8);
    // strcpy(testPage, "Hello!");
    // printf(1, "%s\n", testPage);
    // FreeSharedPage(4);
    // exit();

    // char *testPage = GetSharedPage(4, 8);
    // strcpy(testPage, "Hello child!");
    // if (fork() == 0) {
    //     char *testPage = GetSharedPage(4, 8);
    //     printf(1, "%s\n", testPage);
    //     FreeSharedPage(4);
    //     exit();
    // }
    // wait();
    // FreeSharedPage(4);
    // exit();

    // if (fork() == 0) {
    //     sleep(100);
    //     char *tx = GetSharedPage(0, 6);
    //     char *rx = GetSharedPage(6, 6);
    //     printf(1, "Child!\n");
    // }
    // exit();

    char *tx = GetSharedPage(0, 6);
    char *rx = GetSharedPage(6, 6);
    tx = GetSharedPage(0, 6);
    tx = GetSharedPage(0, 6);

    while (tx[0] != 0) {
    }
    strcpy(tx, "0: Hello!");

    while (rx[0] == 0) {
    }
    printf(1, "0 Received: %s\n", rx);
    memset(rx, 0, 4096);

    while (tx[0] != 0) {
    }
    strcpy(tx, "0: says Goodbye!");

    while (rx[0] == 0) {
    }
    printf(1, "0 Received: %s\n", rx);
    memset(rx, 0, 4096);

    printf(1, "0 %p %d\n", tx, FreeSharedPage(0));
    printf(1, "0 %p %d\n", rx, FreeSharedPage(6));

    exit();
}
