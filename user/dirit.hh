#include <sys/types.h>
#include <stdlib.h>
#include <dirent.h>

class dirit {
public:
  dirit(int fd) : fd_(fd), end_(false) {
    d_ = fdopendir(fd);
    ent_ = (struct dirent*)ebuf_;
    refill();
  }

  ~dirit() {
    closedir(d_);
  }

  dirit& operator ++() {
    refill();
    return *this;
  }

  char *name(char *buf, size_t n) const {
    strncpy(buf, ent_->d_name, n-1);
    buf[n-1] = 0;
    return buf;
  } 

  bool end() const { return end_; }

private:
  void refill(void) {
    struct dirent *result;
    int r = readdir_r(d_, ent_, &result);
    if (r != 0 || result == NULL)
      end_ = true;
  }

  int fd_;
  bool end_;
  char *ebuf_[512];
  struct dirent *ent_;
  DIR *d_;
};

static inline const char*
copy_value(dirit &it)
{
  char *buf = (char*)malloc(256);
  return it.name(buf, 256);
}

static inline void
free_value(dirit &it, const char *name)
{
  free((void*)name);
}
