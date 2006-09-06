#include "types.h"
#include "stat.h"
#include "user.h"

char buf[513];

void
rfile(int fd)
{
  int cc;

  while((cc = read(fd, buf, sizeof(buf) - 1)) > 0){
    buf[cc] = '\0';
    puts(buf);
  }
  if(cc < 0){
    puts("cat: read error\n");
    exit();
  }
}

int
main(int argc, char *argv[])
{
  int fd, i;

  if(argc <= 1) {
    rfile(0);
  } else {
    for(i = 1; i < argc; i++){
      fd = open(argv[i], 0);
      if(fd < 0){
        puts("cat: cannot open ");
        puts(argv[i]);
        puts("\n");
        exit();
      }
      rfile(fd);
      close(fd);
    }
  }

  exit();
}
