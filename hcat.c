#include "types.h"
#include "stat.h"
#include "user.h"

char buf[1];
int col;

void cat(int fd)
{
	int curCol = col;
	int n;

	while((n = read(fd, buf, 1)) > 0) {
		if(buf[0]=='\n'){
			curCol--;
		}
		if (write(1, buf, n) != n) {
			printf(1, "cat: write error\n");
			exit();
		}
		
		if(curCol==0){
			exit();
		}
		
	}
	
	if(n < 0){
		printf(1, "cat: read error\n");
		exit();
	}
}

int canChangeToInteger(char *s) 
{
	int size = strlen(s);
	if (size == 0) return 0; 

	for (int i = 0; i < (int) size; i++) {
		if (s[i] < '0' || s[i] > '9') return 0;  
	}

	return 1; 
}

int main(int argc, char *argv[])
{
	int fd, i;
	
	if(argc<=1){
		cat(0);
		exit();
	}
	
	
	if(canChangeToInteger(argv[1])){
		col = atoi(argv[1]);
	}
	else{
		printf(1, "cat: input error\n");
		exit();
	}
	
	for(i=2;i<argc;i++){
		if((fd=open(argv[i], 0))<0){
			printf(1, "cat: cannot open %s\n", argv[i]);
			exit();
		}
		cat(fd);
		close(fd);
	}
	exit();
}
