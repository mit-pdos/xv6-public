#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include "types.h"
#include "sampler.h"

int
main(int ac, char **av)
{
  static const char *pathname = "perf.data";
  struct logheader *header;
  struct stat buf;
  void *x;
  int fd;
  int i;

  if (ac > 1)
    pathname = av[1];

  fd = open(pathname, O_RDONLY);
  if (fd < 0) {
    perror("open");
    exit(EXIT_FAILURE);
  }
  
  if (fstat(fd, &buf) < 0) {
    perror("fstat");
    exit(EXIT_FAILURE);
  }

  x = mmap(0, buf.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
  if (x == MAP_FAILED) {
    perror("mmap");
    exit(EXIT_FAILURE);
  }
  header = x;

  printf("ncpus %u\n", header->ncpus);
  for (i = 0; i < header->ncpus; i++) {
    printf(" offset %lu size %lu\n",
           header->cpu[i].offset, header->cpu[i].size);
  }

  return 0;
}
