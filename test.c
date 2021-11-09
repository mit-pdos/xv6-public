#include "types.h"
#include "user.h"

int main() {
  getSharedPage(1, 23);
  exit();

    //void* region = getSharedPage(0, 1);
   //
    //// access shm for current process
    ////for (int i = 0; i < 32; i++) {
    ////    struct proc* p = myproc();
    ////    printf(1, p->shm[i].va);
    ////}
    //struct proc *p = myproc();
    //printf(1, "p->shm[0].va: %d\n", p->shm[0].va);
//
    //void* shadd1 = (void*)getSharedPage(0, 6);
//
    //if (fork() == 0) {
    //    void* shadd2 = (void*)getSharedPage(0, 6);
    //    if (shadd1 == shadd2) {
    //        printf(1, "Shared page is shared\n");
    //    } else {
    //        printf(1, "Shared page is not shared\n");
    //    }
    //}
//
    //wait();
    //return 0;
}
