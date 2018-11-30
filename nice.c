#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

int 
main(int argc , char *argv[])
{
	int pid, priority;

	if (argc< 3)
	{
        printf (2, "nice pid Priority\n");

        exit();
	}

	pid = atoi(argv[1]);
	priority = atoi(argv[2]);

	if (priority < 0 || priority > 20)
	{
		printf(2, "Invalid Priority\n");
	}
    
    printf(1, "Pid = %d , Priority = %d\n", pid, priority);
	chpr(pid , priority);
	
    exit();

}