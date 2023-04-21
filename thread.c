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


//sorting algorithm
#define MAX 20
#define THREAD_MAX 4
int a[MAX] = {83, 86, 77, 15, 93, 35, 86, 92, 49, 21, 62, 27, 90, 59, 63, 26, 40, 26, 72, 36
};
int part = 0 ;
// merge function for merging two parts
void merge(int low, int mid, int high)
{
    int left[mid-low+1];
    int right[high - mid];
 
    // n1 is size of left part and n2 is size
    // of right part
    int n1 = mid - low + 1, n2 = high - mid, i, j;
 
    // storing values in left part
    for (i = 0; i < n1; i++)
        left[i] = a[i + low];
 
    // storing values in right part
    for (i = 0; i < n2; i++)
        right[i] = a[i + mid + 1];
 
    int k = low;
    i = j = 0;
 
    // merge left and right in ascending order
    while (i < n1 && j < n2) {
        if (left[i] <= right[j])
            a[k++] = left[i++];
        else
            a[k++] = right[j++];
    }
 
    // insert remaining values from left
    while (i < n1) {
        a[k++] = left[i++];
    }
 
    // insert remaining values from right
    while (j < n2) {
        a[k++] = right[j++];
    }
}

void merge_sort(int low, int high)
{
    // calculating mid point of array
    int mid = low + (high - low) / 2;
    if (low < high) {
 
        // calling first half
        merge_sort(low, mid);
 
        // calling second half
        merge_sort(mid + 1, high);
 
        // merging the two halves
        merge(low, mid, high);
    }
}
 
void* merge_sort123(void* arg)
{
    // which part out of 4 parts
    int thread_part = part++;
 
    // calculating low and high
    int low = thread_part * (MAX / 4);
    int high = (thread_part + 1) * (MAX / 4) - 1;
 
    // evaluating mid point
    int mid = low + (high - low) / 2;
    if (low < high) {
        merge_sort(low, mid);
        merge_sort(mid + 1, high);
        merge(low, mid, high);
    }
    exit();
}
void sorting_test(){
    thread_t threads[THREAD_MAX];
 
    // creating 4 threads
    for (int i = 0; i < THREAD_MAX; i++)
        thread_create(&threads[i], (void*)merge_sort123, 0,CLONE_FILES | CLONE_VM);
 
    // joining all 4 threads
    for (int i = 0; i < 4; i++)
        thread_join(&threads[i]);
 
    // merging the final 4 parts
    merge(0, (MAX / 2 - 1) / 2, MAX / 2 - 1);
    merge(MAX / 2, MAX/2 + (MAX-1-MAX/2)/2, MAX - 1);
    merge(0, (MAX - 1)/2, MAX - 1); 
    // displaying sorted array
    
 printf(1, "Sorted array is \n ");
     for (int i = 0; i < MAX; i++) {
        printf(1, "%d ", a[i]);
    }
    printf(1, "\n");
    exit();
}


struct limits {
	int l, h, res;
};
void *f(void *arg) {
	struct limits *p = (struct limits *)arg;
	int prod = 1, k;
	for(k = p->l; k <= p->h; k++)
		prod *= k;
	p->res = prod;
	exit();
}

void factorial(){
	thread_t tid[2];
	//thread_attr_t pattr[2];
	int  k, n;
	struct limits lim[2];
	//scanf("%d", &n);	
	n = 5;
	lim[0].l = 1;
	lim[0].h = n/2;	
	lim[1].l = n/2 + 1;
	lim[1].h = n;	
	thread_create(&tid[0], (void*)f,  &lim[0],CLONE_VM |  CLONE_FILES);
	thread_create(&tid[1], (void*)f,  &lim[1], CLONE_VM | CLONE_FILES);
	for(k = 0; k < 2; k++) {
		thread_join(&tid[k]);
	}
	printf(1,"Factorial of  %d is %d\n",n, lim[0].res * lim[1].res);
	exit();
}

void test_tid(){
	int tid = gettid();
	printf(1 ,"tid is : %d\n" , tid);
	exit();
}

void test_gettid(){
	thread_t th;
	thread_create(&th, (void*)test_tid,0,0);
	thread_join(&th);
	exit();
}

int main(){
	//ciheckclone();
	//primessum();
	//test for clone files
	//test_clone_cfiles();
	//test for tkill
	//testkill();
	//test_gettid();
	sorting_test();
	//factorial();
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
