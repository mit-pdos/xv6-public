#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "fcntl.h"

#define CLONE_VM 2
#define CLONE_THREAD 1
#define CLONE_FILES 16

int glob = 10;
void userprog(void *arg)
{
	int fd = *((int *)arg);
	/*char buf[15];
	read(fd, buf, sizeof(buf));
        printf(1, "Child process read: %s\n", buf);*/
	glob += 13;
	printf(1, "Global in child %d\n", glob);
	close(fd);
	exit();
}

void child_func(void *arg)
{
	int *fd = (int *)arg;
	printf(1, "child process: fd[0] = %d, fd[1] = %d\n", fd[0], fd[1]);
	close(fd[0]);
	write(fd[1], "hello from child\n", 17);
	close(fd[1]);
	glob += 13;
	printf(1, "Global in child %d\n", glob);
	//close(0);
        close(1);
	exit();
}

void test_CloneVm()
{

	char *stack = malloc(4096);
	clone(&userprog, stack,CLONE_FILES, 0);
	sleep(2);
	glob+=10;
	printf(1, "From parent %d\n", glob);
	exit();
}

void test_CloneFiles(){
	int fd[2];

	if (pipe(fd) < 0)
	{
		exit();
	}
	char *stack = malloc(4096);
	clone(&child_func, stack,CLONE_VM , &fd);
	printf(1, "parent process: fd[0] = %d, fd[1] = %d\n", fd[0], fd[1]);
	// closing the write end of the pipe
	close(fd[1]);
	char buf[256];
	int n = read(fd[0], buf, sizeof(buf));
	write(1, buf, n);
	// closing the read end of the pipe
	close(fd[0]);
	sleep(2);
	exit();

}
int main(int argc, char *argv[])
{
//	test_CloneFiles();
//	test_CloneVm();
/*	char* stack = malloc(4096);
	int tid=clone(&userprog, stack ,CLONE_FILES, 0);
	sleep(2);
	join(tid);*/
/*	sleep(2);*/
//	close(1);
	int fd;
	fd = open("try.txt",O_WRONLY | O_CREATE);
//	close(1);
	char* stack = malloc(4096);
	int th = clone(&userprog ,stack,CLONE_VM,&fd);
	sleep(1);
	join(th);
//	wait();
	char buf[100];
	strcpy(buf, "test file\n");
	int n=write(fd,buf,10);
	if(n<=0){
		printf(1,"write fails in try.txt\n");
	}
	else{
		printf(1,"write succ\n");
	}


	glob +=20;
	printf(1,"parent here glob is:%d\n",glob);
	
	exit();
}
