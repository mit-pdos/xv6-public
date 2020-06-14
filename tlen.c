#include "fcntl.h"
#include "types.h"
#include "user.h"

int main(int argc, char *argv[]) {
	int file1, file2, size, chunk, res, i;
	char *str, *str1;

	if(argc != 2) {
		printf(1, "Incorrect  number of args\n");
		exit();
	}

	// Opening the two files -> one for reading and other for writing
	file1 = open(argv[1], O_RDONLY);
	if(file1 == -1) {
		printf(1, "File not found\n");
		exit();
	}

	file2 = open("copy.txt", O_CREATE | O_RDWR);
	if(file2 == -1) {
		printf(1, "Unable to create copy file\n");
		exit();
	}

	size = lseek(file1, 0, SEEK_END);				//Finding original size of file
	printf(2, "The size of the original file is %d\n", size);

	str = (char *)malloc(sizeof(char) * (size+1));			//Making copy file
	memset(str, 0, size);
	write(file2, str, size);
	free(str);

	if(size%10 == 0)
		chunk = size / 10;
	else
		chunk = (size / 10) + 1;

	//writing chunks in order
	str = (char *)malloc(sizeof(char)*(chunk+1));
	i = 2;
	while(i <= 10) {
		lseek(file1, (i-1)*chunk, SEEK_SET);
		res = read(file1, str, chunk);
		str[res] = '\0';

		lseek(file2, (i-1)*chunk, SEEK_SET);
		write(file2, str, res);

		if(i%2 == 0)
			i--;
		else
			i+=3;
	}
	free(str);
	
	//Checking if two files are the same or not
	str = (char *)malloc(sizeof(char) * (size+1));
	str1 = (char *)malloc(sizeof(char) * (size+1));
	lseek(file1, 0, SEEK_SET);
	lseek(file2, 0, SEEK_SET);
	res = read(file1, str, size);
	str[res] = '\0';
	res = read(file2, str1, size);
	str1[res] = '\0';
	
	if(strcmp(str, str1) == 0) {
		printf(1, "Two files are the same\n\n");
	} else {
		printf(1, "Two files are differentn\n\n");
	}
	free(str);
	free(str1);
	
	//closing files
	close(file1);
	close(file2);
	exit();
}
