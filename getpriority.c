#include "types.h"
#include "user.h"

int main(int argc, char *argv[]) {
	if(argc < 2) {
		printf(2, "usage: getpriority pid\n");
		return -1;
	}
	int pid = atoi(argv[1]);
	int priority = getpriority(pid);
	if(priority < 0) {
		printf(2, "error in getting priority of %d (%d)\n", pid, priority);	
	} else {
		printf(2, "priority of %d = %d\n", pid, priority);
	}
	exit();
}