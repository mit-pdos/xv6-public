#include"types.h"
#include"user.h"
#include"stat.h"
Lock My_Lock;
void function(void* arg1,void* arg2){
    int* X=(int*)arg2;
    Lock_Acquire(&My_Lock);
    printf(2,"Thread %d Finished with value =%d\n",(*X),2*(*X)+1);
    Lock_Release(&My_Lock);
    exit();
}
int main(){
    int l=3;
    int* size=&l;
    int list[3];
    printf(0,"***This Program will calculate 2x+1 for 3 threads where x is the tid passed to thread as its 2nd arg***\n");
    Lock_Init(&My_Lock);
    for(int i=0;i<3;i++){
        list[i]=i+1;
        thread_create(&function,(void*)size,(void*)&list[i]);
    }
    for(int i=1;i<=3;i++){
        join(i);
    }
    exit();
}