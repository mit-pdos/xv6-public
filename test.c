#include "types.h"
#include "user.h"

int main() {
    GetSharedPage(1, 23);
    
    FreeSharedPage(1);
    exit();
    
    //GetSharedPage(1, 64); // should fail

    //theoretical test
    //void* s1 = GetSharedPage(0, 6);
    //struct proc* p = myproc();
    //
    //printf(1, "p->shm[0].va: %d\n", p->shm[0].va);
    //
    //void* p2 = NULL;
    //if (fork() == 0) {
    //    void* s2 = GetSharedPage(0, 6);
    //    struct proc* q = myproc();
    //    p2 = q;
    //    printf(1, "q->shm[0].va: %d\n", q->shm[0].va);
    //}
    //
    //wait();
    ////compare physical address
    //if (p->shm[0].va == p2->shm[0].va) {
    //    printf(1, "Shared page is shared\n");
    //} else {
    //    printf(1, "Shared page is not shared\n");
    //}
    //exit();
}
