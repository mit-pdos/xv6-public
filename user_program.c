#include "types.h"
#include "stat.h"
#include "user.h"

struct proc_info {
    int pid;
    int memsize;
 };

int main(void)
{
	int rc =  fork();
	if( rc == 0){
		//child
		int* childptr = (int *)malloc(1000);
		printf(1,"childptr: %s\n", childptr );
	}
	else{
		//parent
		int* parentptr = (int *)malloc(2000);
		printf(1,"parentptr : %s\n", parentptr);
	}
	getallP();
	wait();
	exit();
}
