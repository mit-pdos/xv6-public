#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

int main(int argc, char *argv[]) {
	
	//handle user type only cp
	if(argc != 3) {
		write(2, "Use: cp src dest\n", 18);
		exit();
	}

	//For source file argv[1] = src.txt
	int src = open(argv[1], O_RDONLY);
	if(src < 0) {
		write(2, "cp: cannot open ", 16);
 		write(2, argv[1], strlen(argv[1]));
    	write(2, "\n", 2);
		exit();
	}

	//For dest file argv[2] = dest.xtx
	int dest = open(argv[2], O_WRONLY | O_CREATE);
	if(dest < 0) {
		write(2, "cp: cannot create ", 18);
		write(2, argv[2], strlen(argv[2]));
		write(2, "\n", 2);
		close(src);
		exit();
	}

	char arr[1024];
	int n;

	//run until EOF
	while((n = read(src, arr, sizeof(arr))) > 0) {
		//ensure all bytes read successfully
		if(write(dest, arr, n) != n) {
			write(2, "cp: write error\n", 17);
        	close(src);
        	close(dest);
        	exit();
    	}
	}

	//Handle error
	if(n < 0) {
		write(2, "cp: read error\n", 16);
	}

	close(src);
	close(dest);

	exit();
}