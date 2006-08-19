#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"

char buf[512];
struct stat st;
struct dirent dirent;

int
main(int argc, char *argv[])
{
  int fd;
  uint off;
  uint sz;
  int i;

  if(argc > 2){
    puts("Usage: ls [dir]\n");
    exit();
  }

  if (argc == 2) {
    fd = open(argv[1], 0);
    if(fd < 0){
      printf(2, "ls: cannot open dir %s\n", argv[1]);
      exit();
    } 
  } else {
    fd = open(".", 0);
    if(fd < 0){
      printf(2, "ls: cannot open .\n");
      exit();
    }
  }

  if (fstat(fd, &st) < 0) {
    printf(2, "ls: cannot stat dir\n");
    exit();
  }
  if (st.st_type != T_DIR) {
    printf(2, "ls: dir is not a directory\n");
  }
  sz = st.st_size;
  for(off = 0; off < sz; off += sizeof(struct dirent)) {
    if (read(fd, &dirent, sizeof(struct dirent)) != sizeof(struct dirent)) {
      printf(1, "ls: read error\n");
      break;
    }
    if (dirent.inum != 0) {
      // xxx prepend to name the pathname supplied to ls (e.g. .. in ls ..)
      if (stat (dirent.name, &st) < 0)  {
	printf(1, "stat: failed %s\n", dirent.name);
	continue;
      }
      for (i = 0; i < DIRSIZ; i++) {
	if (dirent.name[i] != '\0')
	  printf(1, "%c", dirent.name[i]);
	else
	  printf(1, " ");
      }
      printf(1, "%d %d %d\n", st.st_type, dirent.inum, st.st_size);
    }
  }
  close(fd);

  exit();
}
