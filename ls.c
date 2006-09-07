#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"

char buf[512];
struct stat st;
struct dirent dirent;

void
pname(char *n)
{
  int i;

  for(i = 0; (i < DIRSIZ) && (n[i] != '\0') ; i++) {
      printf(1, "%c", n[i]);
  }
  for(; i < DIRSIZ; i++)
    printf(1, " ");
}

int
main(int argc, char *argv[])
{
  int fd;
  uint off;
  uint sz;

  if(argc > 2){
    puts("Usage: ls [dir]\n");
    exit();
  }

  if(argc == 2) {
    fd = open(argv[1], 0);
    if(fd < 0){
      printf(2, "ls: cannot open %s\n", argv[1]);
      exit();
    }
  } else {
    fd = open(".", 0);
    if(fd < 0){
      printf(2, "ls: cannot open .\n");
      exit();
    }
  }

  if(fstat(fd, &st) < 0) {
    printf(2, "ls: cannot stat dir\n");
    exit();
  }

  switch(st.type) {
  case T_FILE:
    pname(argv[1]);
    printf(1, "%d %d %d\n", st.type, st.ino, st.size);
    break;
  case T_DIR:
    sz = st.size;
    for(off = 0; off < sz; off += sizeof(struct dirent)) {
      if(read(fd, &dirent, sizeof dirent) != sizeof dirent) {
        printf(1, "ls: read error\n");
        break;
      }
      if(dirent.inum != 0) {
        // xxx prepend to name the pathname supplied to ls (e.g. .. in ls ..)
        if(stat (dirent.name, &st) < 0) {
          printf(1, "stat: failed %s\n", dirent.name);
          continue;
        }
        pname(dirent.name);
        printf(1, "%d %d %d\n", st.type, dirent.inum, st.size);
      }
    }
    break;
  }
  close(fd);
  exit();
}
