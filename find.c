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
find(char *filename,char *path)
{
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  if((fd = open(path, 0)) < 0){
    printf(2, "find: cannot open %s\n", path);
    return;
  }

  if(fstat(fd, &st) < 0)
  {
    printf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }

switch(st.type)
  {
  case T_FILE:
    printf(1, "%s %d %d %d\n", fmtname(path), st.type, st.ino, st.size);
    break;

  case T_DIR:
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){ 
      printf(1, "find: path too long\n");
      break;
  } 
    strcpy(buf, path);
    p = buf+strlen(buf);
    *p++ = '/';
    while(read(fd, &de, sizeof(de)) == sizeof(de))
    {
      if(de.inum == 0)
        continue;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      if(stat(buf, &st) < 0)
      {
        printf(1, "find: cannot stat %s\n", buf);
        continue;
      }
      if (strcmp(de.name,".") == 0 || strcmp(de.name,"..")==0);
      printf(1, "%s %d %d %d\n", fmtname(buf), st.type, st.ino, st.size);
    }
    break;
  } 
close(fd);
}

int 
main (int argc, char *argv[])
   {
    if(argc !=4 || strcmp(argv[2],"-name") !=0)
    {
       //printf(2, "Usage: %s <filename>\n");
        printf(2, "Usage: find <directory> -name <filename>\n");
        exit();
    }
    find (argv[1],argv[3]);
    exit();
    }

    // Inside the find function in find.c
if (argc > 1 && strcmp(argv[1], "-printf") == 0) {
    // Check if there is a format string after -printf
    if (argc > 2) {
        char *formatString = argv[2];
        // Call a function to print files based on the format string
        printFilesWithFormat(dirpath, formatString);
    } else {
        printf(2, "Usage: find <directory> -printf <format>\n");
    }
    exit();
}
