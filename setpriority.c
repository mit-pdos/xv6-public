#include "types.h"
#include "user.h"

int main(int argc, char *argv[]) {
	if(argc < 3) {
		printf(2, "usage: setpriority pid priority\n");
		return -1;
	}
	int pid = atoi(argv[1]);
	int priority = atoi(argv[2]);
	int result = setpriority(pid, priority);

	if(result < 0) {
		printf(2, "failed to set priority of %d (%d)\n", pid, result);
		return -1;
	}
	printf(2, "set priority of %d to %d\n", pid, priority);
	return 0;
}