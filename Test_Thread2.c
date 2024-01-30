#include"types.h"
#include"user.h"
#include"stat.h"
Lock P2_Perm,P1_Perm;
void p1(void* arg1,void* arg2){
    printf(0,"A\n");
    Lock_Release(&P2_Perm);
    Lock_Acquire(&P1_Perm);
    printf(0,"C\n");
    printf(0,"B\n");
    exit();
}

void p2(void* arg1,void* arg2){
    Lock_Acquire(&P2_Perm);
    printf(0,"F\n");
    Lock_Release(&P1_Perm);
    printf(0,"E\n");
    printf(0,"G\n");
    exit();
}
int main(){
    Lock_Init(&P2_Perm);
    Lock_Init(&P1_Perm);
    Lock_Acquire(&P2_Perm);
    Lock_Acquire(&P1_Perm);
    thread_create(&p1,(void*)0,(void*)0);
    thread_create(&p2,(void*)0,(void*)0);
    join(1);
    join(2);
    exit();
}