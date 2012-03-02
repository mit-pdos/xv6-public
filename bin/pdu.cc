#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "wq.hh"

void
du(int fd)
{
  struct stat st;

  if (fstat(fd, &st) < 0) {
    fprintf(2, "du: cannot stat\n");
    close(fd);
    // return 0;
  }

  int sz = st.size;
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
      if (nfd >= 0) {
        // size += du(nfd);
	struct work *w = (struct work *) malloc(sizeof(struct work));
	w->rip = (void*) du;
	w->arg0 = (void *) nfd;
	wq_push(w);
      }
    }
  }

  close(fd);
  size += sz;
}

int
main(int ac, char **av)
{
  wq_init(4);
  struct work *w = (struct work *) malloc(sizeof(struct work));
  w->rip = (void*) du;
  w->arg0 = (void *) open(".", 0);
  wq_push(w);

  // size = du(open(".", 0)));

  wq_start();

  printf("%d\n", size);
  exit();
}
