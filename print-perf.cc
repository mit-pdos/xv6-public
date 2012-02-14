#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include "types.h"
#include "sampler.h"

#include <unordered_map>
#include <map>

struct gt
{
  bool operator()(int x0, int x1) const
  {
    return x0 > x1;
  }
};

int
main(int ac, char **av)
{
  static const char *pathname = "sampler";
  struct logheader *header;
  struct stat buf;
  char *x;
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

  x = (char*)mmap(0, buf.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
  if (x == MAP_FAILED) {
    perror("mmap");
    exit(EXIT_FAILURE);
  }
  header = (struct logheader*)x;

  printf("ncpus %u\n", header->ncpus);
  for (i = 0; i < header->ncpus; i++) {
    printf(" offset %lu size %lu\n",
           header->cpu[i].offset, header->cpu[i].size);
  }

  std::unordered_map<u64, int> map;
  for (i = 0; i < header->ncpus; i++) {
    struct pmuevent *p;
    struct pmuevent *q;

    p = (struct pmuevent*)(x + header->cpu[i].offset);
    q = (struct pmuevent*)(x + header->cpu[i].offset + header->cpu[i].size);
    for (; p < q; p++) {
      auto it = map.find(p->rip);
      if (it == map.end())
        map[p->rip] = 1;
      else
        it->second = it->second + 1;
    }
  }

  std::map<int, u64, gt> sorted;
  for (std::pair<const u64, int> &p : map)
    sorted[p.second] = p.first;

  for (std::pair<const int, u64> &p : sorted)
    printf("%lx %u\n", p.second, p.first);

  return 0;
}
