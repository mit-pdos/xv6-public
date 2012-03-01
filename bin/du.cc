#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"

static int
du(int fd)
{
  struct stat st;
  if (fstat(fd, &st) < 0) {
    fprintf(2, "du: cannot stat\n");
    close(fd);
    return 0;
  }

  int size = st.size;
  if (st.type == T_DIR) {
    struct dirent de;
    while (read(fd, &de, sizeof(de)) == sizeof(de)) {
      if (de.inum == 0)
        continue;

      char buf[DIRSIZ+1];
      memmove(buf, de.name, DIRSIZ);
      buf[DIRSIZ] = 0;

      if (!strcmp(buf, ".") || !strcmp(buf, ".."))
        continue;

      int nfd = openat(fd, buf, 0);
      if (nfd >= 0)
        size += du(nfd);  // should go into work queue
    }
  }

  close(fd);
  return size;
}

int
main(int ac, char **av)
{
  printf("%d\n", du(open(".", 0)));
  exit();
}
