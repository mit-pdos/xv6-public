#define __STDC_FORMAT_MACROS

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include <inttypes.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <unordered_map>
#include <map>

#include "types.h"
#include "sampler.h"

static void __attribute__((noreturn)) 
edie(const char* errstr, ...) 
{
  va_list ap;

  va_start(ap, errstr);
  vfprintf(stderr, errstr, ap);
  va_end(ap);
  fprintf(stderr, ": %s\n", strerror(errno));
  exit(EXIT_FAILURE);
}

class Addr2line {
private:
  int _out, _in;
  
public:
  explicit Addr2line(const char* path);
  ~Addr2line();
  int lookup(uint64_t pc, char** func, char** file, int* line) const;
};

#ifdef __APPLE__
static char* xstrndup(const char* str, size_t len)
{
  char* r;
  
  r = (char*) malloc(len + 1);
  if (r == NULL)
    return r;
  memcpy(r, str, len);
  r[len] = 0;
  return r;
}
#else
#define xstrndup strndup
#endif

Addr2line::Addr2line(const char* path)
{
  static const char* addr2line_exe[] = {
    "addr2line",
    "x86_64-jos-elf-addr2line",
  };
  
  int out[2], in[2], check[2], child, r;
  
  if (pipe(out) < 0 || pipe(in) < 0 || pipe(check) < 0)
    edie("%s: pipe", __func__);
  if (fcntl(check[1], F_SETFD,
            fcntl(check[1], F_GETFD, 0) | FD_CLOEXEC) < 0)
    edie("%s: fcntl", __func__);
  
  child = fork();
  if (child < 0) {
    edie("%s: fork", __func__);
  } else if (child == 0) {
    unsigned int i;
    
    close(check[0]);
    dup2(out[0], 0);
    close(out[0]);
    close(out[1]);
    dup2(in[1], 1);
    close(in[0]);
    close(in[1]);
    
    for (i = 0; i < sizeof(addr2line_exe) / sizeof(addr2line_exe[0]); i++)
      r = execlp(addr2line_exe[i], addr2line_exe[i],
                 "-C", "-f", "-e", path, NULL);
    r = 1;
    assert(sizeof(r) == write(check[1], &r, sizeof(r)));
    exit(0);
  }
  close(out[0]);
  close(in[1]);
  close(check[1]);
  
  if (read(check[0], &r, sizeof(r)) != 0) {
    errno = r;
    edie("%s: exec", __func__);
  }
  close(check[0]);
  
  _out = out[1];
  _in = in[0];
}

Addr2line::~Addr2line()
{
  close(_in);
  close(_out);
}

int
Addr2line::lookup(uint64_t pc, char** func, char** file, int* line) const
{
  char buf[4096];
  int n = snprintf(buf, sizeof(buf), "%#"PRIx64"\n", pc);
  if (n != write(_out, buf, n))
    edie("%s: write", __func__);

  n = 0;
  while (1) {
    int r = read(_in, buf + n, sizeof(buf) - n - 1);
    if (r < 0)
      edie("%s: read", __func__);
    n += r;
    buf[n] = 0;
    
    int nls = 0;
    for (int i = 0; i < n; ++i)
      if (buf[i] == '\n')
        nls++;
    if (nls >= 2)
      break;
  }
  
  *func = *file = NULL;
  
  char* nl, *col, *end;
  nl = strchr(buf, '\n');
  *func = xstrndup(buf, nl - buf);
  col = strchr(nl, ':');
  if (!col)
    goto bad;
  *file = xstrndup(nl + 1, col - nl - 1);
  end = NULL;
  *line = strtol(col + 1, &end, 10);
  if (!end || *end != '\n')
    goto bad;
  
  return 0;
  
bad:
  free(*func);
  free(*file);
  *func = *file = NULL;
  *line = 0;
  return -1;
}

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
  static const char *sample;
  static const char *elf;
  struct logheader *header;
  struct stat buf;
  char *x;
  int fd;
  int i;

  if (ac < 3)
    edie("usage: %s sample-file elf-file", av[0]);

  sample = av[1];
  elf = av[2];

  fd = open(sample, O_RDONLY);
  if (fd < 0) {
    perror("open");
    exit(EXIT_FAILURE);
  }

  Addr2line addr2line(elf);
  
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

  for (std::pair<const int, u64> &p : sorted) {
    char *func;
    char *file;
    int line;
    addr2line.lookup(p.second, &func, &file, &line);
    printf("%-10u %016"PRIx64" %s:%u %s\n", 
           p.first, p.second, file, line, func);
    free(func);
    free(file);
  }

  return 0;
}
