#include "types.h"
#include "user.h"

// test the scheduler's performance
// two tests: "a" and "b"
// test "a" tests the scheduler on many forks of short lived processes
// test "b" tests the scheduler on few forks of long running processes
int main(int argc, char **argv) {
	if(argc < 2) {
		printf(2, "error: usage test_scheduler a|b\n");
		exit();
	}
	char test = *argv[1];
	int startticks = uptime();
	int pid = fork();
	if(pid) {
		wait();
		int tickstaken = uptime()-startticks;
		printf(2, "test '%c' took %d ticks\n", test, tickstaken);
	} else {
		if(test == 'a') {
			for(int i = 0;i < 400;i++) {
				if(fork()) {
					wait();
				} else {
					int j = 0;
					for(int i = 0;i < 10000;i++) {
						j = j+i;
					}
					printf(2, "result is: %d\n", j);
					break;
				}
			}
		} else {
			for(int i = 0;i < 10;i++) {
				if(fork()) {
					wait();
				} else {
					int j = 0;
					for(int i = 0;i < 10000000;i++) {
						j = j+i;
						if(i % 100000 == 0) {
							sleep(1);
						}
					}
					printf(2, "result is: %d\n", j);
					break;
				}
			}
		}
	}
	exit();
}