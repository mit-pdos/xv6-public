#include "types.h"
#include "user.h"


int
main(int argc, char *argv[])
{
	if (argc != 2){
				printf(1, "Usage: SMLsanity <n>\n");
				exit();
 		}
	int i,j,k; //迴圈的索引值
	int n; //3n個process
	int ready_t;
	int running_t;
	int sleeping_t;
	int sums[3][3];
	for (i = 0; i < 3; i++)
		for (j = 0; j < 3; j++)
			sums[i][j] = 0;
	n = atoi(argv[1]);
	i = n; //unimportant
	int Pid;
	for (i = 0; i < 3 * n; i++) 
	{
		j = i % 3;
		Pid = fork();
		if (Pid == 0) 
		{//child
			int C_id = (getpid() - 4) % 3; // 在下次fork時確保獨立於上次fork的Pid
			#ifdef SML
			switch(C_id) 
			{
				case 0:
					set_prio(1); break;
				case 1:
					set_prio(2); break;
				case 2:
					set_prio(3); break;
			}
			#endif
			for (k = 0; k < 100; k++)
				for (j = 0; j < 1000000; j++){}
			exit(); // children exit
		}
		continue; // father continues (fork下一個)
	}
	for (i = 0; i < 3 * n; i++) 
	{
		Pid = wait2(&ready_t, &running_t, &sleeping_t);
		int C_id = (Pid - 4) % 3; // 找尋下個Pid時確保獨立於上次的pid
		switch(C_id) 
		{
			case 0: // CPU bound processes
				printf(1, "Priority 1, pid: %d, ready: %d, running: %d, sleeping: %d, turnaround: %d\n",
					 pid, ready_t, running_t, sleeping_t, ready_t + running_t + sleeping_t);
				sums[0][0] += ready_t;
				sums[0][1] += running_t;
				sums[0][2] += sleeping_t;
				break;
			case 1: // CPU bound processes, short tasks
				printf(1, "Priority 2, pid: %d, ready: %d, running: %d, sleeping: %d, turnaround: %d\n",
					 pid, ready_t, running_t, sleeping_t, ready_t + running_t + sleeping_t);
				sums[1][0] += ready_t;
				sums[1][1] += running_t;
				sums[1][2] += sleeping_t;
				break;
			case 2: // simulating I/O bound processes
				printf(1, "Priority 3, pid: %d, ready: %d, running: %d, sleeping: %d, turnaround: %d\n",
					 pid, ready_t, running_t, sleeping_t, ready_t + running_t + sleeping_t);
				sums[2][0] += ready_t;
				sums[2][1] += running_t;
				sums[2][2] += sleeping_t;
				break;
		}
	}
	for (i = 0; i < 3; i++)
		for (j = 0; j < 3; j++)
			sums[i][j] /= n;
  	printf(1, "\n\nPriority 1:\nAverage ready time: %d\nAverage running time: %d\nAverage sleeping time: %d\nAverage turnaround time: %d\n\n\n",
  	 sums[0][0], sums[0][1], sums[0][2], sums[0][0] + sums[0][1] + sums[0][2]);
	printf(1, "Priority 2:\nAverage ready time: %d\nAverage running time: %d\nAverage sleeping time: %d\nAverage turnaround time: %d\n\n\n",
		 sums[1][0], sums[1][1], sums[1][2], sums[1][0] + sums[1][1] + sums[1][2]);
	printf(1, "Priority 3:\nAverage ready time: %d\nAverage running time: %d\nAverage sleeping time: %d\nAverage turnaround time: %d\n\n\n",
		 sums[2][0], sums[2][1], sums[2][2], sums[2][0] + sums[2][1] + sums[2][2]);
	exit();
}
