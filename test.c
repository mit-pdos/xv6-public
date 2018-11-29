#include"types.h"
#include"stat.h"
#include"user.h"
#include"signal.h"

//static int count=0;

void handle_signal(int signum)
{
	printf(1,"Caught signal %d\n",signum);
	if(signum==SIGFPE)
		printf(1,"test pass\n");
	else 
		printf(1,"test failed: signal sent %d\n",signum);
	exit();
}

int main(int argc,char *argv[])
{
	int x=2;
	int y=0;

	handle_signal(signal(SIGFPE,handle_signal));
	x=x/y;	

	printf(2,"getpid is: %d\n",getpid());
	exit();

}
