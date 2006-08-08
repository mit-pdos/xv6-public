#include "user.h"

char buf[513];

int
main(int argc, char *argv[])
{
  int fd, i, cc;

  if(argc < 2){
    puts("Usage: cat files...\n");
    exit();
  }

  for(i = 1; i < argc; i++){
    fd = open(argv[i], 0);
    if(fd < 0){
      puts("cat: cannot open ");
      puts(argv[i]);
      puts("\n");
      exit();
    }
    while((cc = read(fd, buf, sizeof(buf) - 1)) > 0){
      buf[cc] = '\0';
      puts(buf);
    }
    if(cc < 0){
      puts("cat: read error\n");
      exit();
    }
    close(fd);
  }

  exit();
}
