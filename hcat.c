#include "types.h"
#include "stat.h"
#include "user.h"

char buf[512];

void
hcat(int fd, int length)
{
  int i, n, size = 0, max_size;
  char* offset;
  char hbuf[512] = {0};

  while((max_size = read(fd, buf, sizeof(buf))) > 0) {
	offset = &buf[0];
	for (i = 0; i < length; i++) {
	  strcpy(hbuf, offset);
	  n = strchr(offset, '\n') - offset + 1;
	  hbuf[n] = '\n';
	  if (write(1, hbuf, n) != n) {
	    exit();
      } else if ((size += n) > max_size) {
		exit();
	  }
	  offset += n;
    }
	if (i == length) {
	  exit();
	}
  }
  if(max_size < 0){
    printf(1, "hcat: read error\n");
    exit();
  }
}

int
main(int argc, char *argv[])
{
  int fd, i;
  int length;

  if(argc <= 2){
	printf(1, "Usage: hcat [line #] [file..]\n");
    exit();
  }

  length = atoi(argv[1]);

  if(length < 0){
	printf(1, "Usage: hcat [line #] [file..]\n");
	exit();
  }

  for(i = 2; i < argc; i++){
    if((fd = open(argv[i], 0)) < 0){
      printf(1, "hcat: cannot open %s\n", argv[i]);
      exit();
    }
    hcat(fd, length);
    close(fd);
  }
  exit();
}
