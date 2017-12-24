#include "types.h"
#include "user.h"


int
main(int argc, char *argv[])
{
	if (argc != 2){
		printf(1, "Usage: sanity <n>\n");
		exit();
 	}
	int i;
	int n;
	int j = 0;
	int k;
	int retime;
	int rutime;
	int stime;
	int sums[2][3];
	for (i = 0; i < 2; i++)
		for (j = 0; j < 3; j++)
			sums[i][j] = 0;
	n = atoi(argv[1]);
	i = n; //unimportant
	int pid;
	for (i = 0; i < 2 * n; i++) {
		j = i % 2;
		pid = fork();
		if (pid == 0) {//child
			j = (getpid() - 4) % 2; // ensures independence from the first son's pid when gathering the results in the second part of the program
			switch(j) {
				case 0: //CPUâbound process (CPU):
					for (k = 0; k < 100; k++){
						for (j = 0; j < 1000000; j++){}
					}
				case 1:// simulate I/O bound process (IO)
				for(k = 0; k < 1; k++){
					sleep(1);
				}
				break;
			}
			exit(); // children exit here
		}
		continue; // father continues to spawn the next child
	}
	for (i = 0; i < 2 * n; i++) {
		pid = wait2(&retime, &rutime, &stime);
		int res = (pid) % 2; // correlates to j in the dispatching loop
		switch(res) {
			case 0: // CPU bound processes
			printf(1, "CPU-bound, pid: %d, ready: %d, running: %d, sleeping: %d, turnaround: %d\n", pid, retime, rutime, stime, retime + rutime + stime);
			sums[0][0] += retime;
			sums[0][1] += rutime;
			sums[0][2] += stime;
			break;
			case 1: // simulating I/O bound processes
			printf(1, "I/O-bound, pid: %d, ready: %d, running: %d, sleeping: %d, turnaround: %d\n", pid, retime, rutime, stime, retime + rutime + stime);
			sums[1][0] += retime;
			sums[1][1] += rutime;
			sums[1][2] += stime;
		break;
		}
	}
	for (i = 0; i < 2; i++)
		for (j = 0; j < 3; j++)
			sums[i][j] /= n;
	printf(1, "\n\nCPU bound:\nAverage ready time: %d\nAverage running time: %d\nAverage sleeping time: %d\nAverage turnaround time: %d\n\n\n", sums[0][0], sums[0][1], sums[0][2], sums[0][0] + sums[0][1] + sums[0][2]);
	printf(1, "\n\nI/O bound:\nAverage ready time: %d\nAverage running time: %d\nAverage sleeping time: %d\nAverage turnaround time: %d\n\n\n", sums[1][0], sums[1][1], sums[1][2], sums[1][0] + sums[1][1] + sums[1][2]);
	exit();
}
