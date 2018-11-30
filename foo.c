#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

int
main(int argc, char *argv[])
{
	int k, n, id;
	double x = 0, z, d;

	if (argc< 2)
		n = 1;
	else
		n = atoi( argv[1] );

	if (n< 0 || n> 20)
		n = 2;

	if (argc< 3)
		d = 1.0;
	else
		d = atoi(argv[2]);

	x = 0;
	id = 0;
	for(k = 0 ; k< n ;k++)
	{
		id = fork();
		if (id< 0)
		{
			printf(1, "Parent %d fork failed!\n", getpid());
		}

		else if (id> 0)
		{
			printf(1, "Parent %d create Child %d\n", getpid(), id);
			wait();
		}

		else
		{
			printf(1, "Child %d is created\n", getpid());
			for (z = 0; z< 8000000.0;z += d)
			{
				x = x + 2.99 * 85.42;
			}
			break;
		}
	}

	exit();
}
