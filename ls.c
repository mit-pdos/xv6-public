#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"

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
  char buf[512], *p;
  int fd;
  uint off, sz;
  struct dirent de;
  struct stat st;

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
    for(off = 0; off < sz; off += sizeof(de)) {
      if(read(fd, &de, sizeof(de)) != sizeof(de)) {
        printf(1, "ls: read error\n");
        break;
      }
      if(de.inum != 0) {
        p = buf;
        if(argc == 2) {			  
          strcpy(p, argv[1]);
          p += strlen(p);
          if(*(p-1) != '/')
            *p++ = '/';
        }
        memmove(p, de.name, DIRSIZ);
        p[DIRSIZ] = 0;
        if(stat(buf, &st) < 0) {
          printf(1, "stat: failed %s\n", de.name);
          continue;
        }
        pname(de.name);
        printf(1, "%d %d %d\n", st.type, de.inum, st.size);
      }
    }
    break;
  }
  close(fd);
  exit();
}
