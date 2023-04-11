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
   // int p=*(int *)arg;
    glob +=13;
    printf(1,"Global in routine %d\n",glob);
    exit();
}


int main(){
	thread_t th1;
	thread_create(&th1,routine ,0, CLONE_VM);
	thread_join(&th1);
	glob += 10;
	printf(1,"Global in Parent %d\n",glob);
	exit();

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

        return thread->tid;
}

int thread_join(thread_t *th){
	sleep(2);
	return 0;
}
