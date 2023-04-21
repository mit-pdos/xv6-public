#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "fcntl.h"


#define CLONE_VM 2
#define CLONE_THREAD 1
#define CLONE_FILES 16

int glob = 10;

typedef struct thread_t{
        unsigned int tid;
        void* stack;
}thread_t;

int thread_create(thread_t* ,int(*start_routine)(void*),void*, int );

int thread_join(thread_t*);

int routine(void *arg){
    int p=*(int *)arg;
    glob +=13;
    printf(1,"Global in routine %d\n",glob);
    printf(1,"Argument in p:%d\n",p);
    exit();
}

int checkclone(){
    thread_t th1;
    int ar = 10;
    thread_create(&th1,routine, &ar,CLONE_VM | CLONE_FILES);
    thread_join(&th1);
    glob +=10;
    printf(1,"Global in Checkclone %d\n",glob);
    exit();
}

//testing program
int primes[10] = {2,3,5,7,11,13,17,19,23,29};
int globalSum = 0;

int findsum(void* arg){
	int index = *(int*)arg;
	int sum =0;
	printf(1,"index is %d\n",index);
	for(int j = 0;j<5;j++){
		sum += primes[index+j];
	}
//	*(int*)arg = sum;
	printf(1,"Local sum is %d\n",sum);
	globalSum += sum;
//	return arg;
	exit();
}

void* primessum(){
	thread_t th[2];
	int a[2];
	for(int i=0 ;i<2;i++){
		//int* a = malloc(sizeof(int));
		//*a = i*5;
		a[i] = i*5;
		thread_create(&th[i],findsum,a+i,CLONE_FILES | CLONE_VM);

//		free(a);
	}
	for(int i=0; i<2;i++){
		thread_join(&th[i]);
	}
	printf(1,"Global Sum of primes is %d \n",globalSum);
	exit();

}

int waitthread(void* arg){
	int tid = (int)arg;
	int i = 0;
	printf(1,"Thread %d started\n",tid);
	while(i<10){
		sleep(1000000);
		printf(1,"while : %d\n" , i);
		i++;
	}
	exit();
}

void testkill(){
	/*thread_t th[2];
	int i;
	int a[2];
	for(i = 0;i<2 ;i++){
		a[i] = i;
		thread_create(&th[i],waitthread,a+i,CLONE_VM | CLONE_FILES);
		printf(1,"Thread %d created with tid %d\n",i,th[i].tid);
	}

	sleep(100);

	if(tkill(th[1].tid)<0){
		printf(1,"Error killing thread\n");
		exit();
	}
	printf(1,"Thread 1 killed\n");

	for(i = 0 ; i<2 ; i++){
		if(thread_join(&th[i])<0){
			printf(1,"Error joining thread\n");
			exit();
		}
	}
	printf(1,"Thread 0 compleleted\n");
	exit();*/
	
	
	thread_t th;
	thread_create(&th,waitthread,0,CLONE_VM | CLONE_FILES);
	printf(1,"Thread created with tid %d\n",th.tid);
	
	if(tkill(th.tid)<0){
		printf(1,"Error killing thread\n");
		exit();
	}
	printf(1,"Thread killed\n");
	thread_join(&th);
	exit();
}

int readData(void *arg){
	char buf[18];
    int fd=*(int *)arg;
    
    read(fd , buf , 8);
	printf(1,"child : %s\n",buf);
    exit();
}

void test_clone_files(){
	thread_t th;
	char buf[18];
	int fd = open("README",O_RDONLY);
	printf(1 , "fd : %d\n" , fd);
	thread_create(&th,readData,&fd,0);
	thread_join(&th);
	read(fd , buf , 8);
	printf(1,"parent : %s\n",buf);
	exit();
}

int main(){
	//ciheckclone();
	//primessum();
	//test for clone files
	//test_clone_files();
	//test for tkill
	testkill();
}

int thread_create(thread_t* thread, int(*start_routine)(void*),void *arg, int flags)
{
        thread->stack = malloc(4096);
        if(!thread->stack){
                printf(1,"malloc failed\n");
                return -1;
        }
        thread->tid = clone((void*)start_routine, thread->stack,flags , arg);
        if( thread->tid == -1){
        //      free(thread->stack);
                return -1;
        }
        return 0;
}

int thread_join(thread_t *th){

	return join(th->tid);

}
