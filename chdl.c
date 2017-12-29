#include "types.h"
#include "stat.h"
#include "user.h"

int 
main(int argc,char *argv[])
{
	int deadline, pid;
	
	if(argc < 3)
	{
    printf(2,"Usage:  pid deadline\n");
    exit();
	}  
	pid=atoi(argv[1]);
	deadline = atoi(argv[2]);

	if(deadline < 0)
	{
    printf(2,"Invalid deadline\n");
    exit();
	}
	printf(1,"pid = %d,deadline = %d\n",pid,deadline);
	chdl(pid,deadline);
	exit();
}

