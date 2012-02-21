typedef struct fstream {
  int fd;
  off_t off;
  off_t poff;
  struct stat stat;
  int err:1;
  int eof:1;
  int pfill:1;
} FILE;

FILE  *fdopen(int fd, const char *mode);
int    fclose(FILE *fp);
size_t fread(void *ptr, size_t size, size_t nmemb, FILE *fp);
int    feof(FILE *fp);
int    ferror(FILE *fp);
