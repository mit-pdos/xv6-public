#include"types.h"
#include"user.h"
#include"stat.h"
void Print(void* arg1,void* arg2){
    int* X=(int*)arg2;
    printf(1,"***Thread****%d\n",*X);
    exit();
}
int main(){
    int l=3;
    int* size=&l;
    int tid,Stat;
    int list[3];
    for(int i=0;i<3;i++){
        list[i]=i+1;
        tid=thread_create(&Print,(void*)size,(void*)&list[i]);
    }
    for(int i=1;i<=3;i++){
        Stat=thread_join(i);
    }
    printf(2,"%d %d",tid,Stat);
    exit();
}