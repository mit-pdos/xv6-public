#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"

char*
fmtname(char *path)
{
  static char buf[DIRSIZ+1];
  char *p;

  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  if(strlen(p) >= DIRSIZ)
    return p;
  memmove(buf, p, strlen(p));
  memset(buf+strlen(p), ' ', DIRSIZ-strlen(p));
  return buf;
}

void
dw(char *path)
{
  char buf[512], *p;
  int fd;
  int skip = 2; 
  struct dirent de;
  struct stat st;

  if((fd = open(path, 0)) < 0){
    printf(2, "dw: cannot open %s\n", path);
    return;
  }

  if(fstat(fd, &st) < 0){
    printf(2, "dw: cannot stat %s\n", path);
    close(fd);
    return;
  }

  switch(st.type){
  case T_FILE:
    printf(1, "%s %d %d %d\n", fmtname(path), st.type, st.ino, st.size);
    break;

  case T_DIR:
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
      printf(1, "dw: path too long\n");
      break;
    }
    strcpy(buf, path);
    p = buf+strlen(buf);
    *p++ = '/';
    while(read(fd, &de, sizeof(de)) == sizeof(de)){
      if(de.inum == 0)
        continue;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      if(stat(buf, &st) < 0){
        printf(1, "dw: cannot stat %s\n", buf);
        continue;
      }
      switch(st.type){
      case T_FILE:
        printf(1, "%s type: %d inode: %d size: %d\n", fmtname(buf), st.type, st.ino, st.size);
        break;
      case T_DIR: 
        if(skip){
          //since this relies on xv6's ls.c,
          // we need to skip the first two (the directory itself and the parent) to not infinitly recurse
          skip--;
          break;
        }
        printf(1, "%s type: %d inode: %d size: %d\n", fmtname(buf), st.type, st.ino, st.size);
        //now we recurse 
      dw(buf);
      break;
      }
    }
    break;
  }
  close(fd);
}

int
main(int argc, char *argv[])
{
  int i;

  if(argc < 2){
    dw(".");
    exit();
  }
  for(i=1; i<argc; i++)
    dw(argv[i]);
  exit();
}
