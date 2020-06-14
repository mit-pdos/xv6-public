#include "fcntl.h"
#include "types.h"
#include "user.h"

int main(int argc, char *argv[]) {
	int file, off;
	int len, res;
	char *str;

	if(argc != 5) {
		printf(1, "Incorrect number of arguments\n");
		exit();
	}

	//opening the given file
	file = open(argv[1], O_RDWR);
	if(file == -1) {
		printf(1, "File not found\n");
		exit();
	}

	//seek to the given location
	off = atoi(argv[2]);
	lseek(file, off, SEEK_SET);

	//read the required bytes
	len = atoi(argv[3]);
	str = (char *)malloc(sizeof(char) * (len+1));
	res = read(file, str, len);
	str[res] = '\0';

	//compare with the given string
	if(strcmp(argv[4], str) == 0) {
		printf(1, "String are same. Lseek works\n");
	}
	else {
		printf(1, "Strings are different.\n");
	}
	close(file);
	free(str);
	exit();
}
