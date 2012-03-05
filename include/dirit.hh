class dirit {
public: 
  dirit(int fd) : fd_(fd), end_(false) {
    refill();
  }

  ~dirit() {
    close(fd_);
  }

  dirit& operator ++() {
    refill();
    return *this;
  }

  const char * operator*() {
    char *buf = (char*)malloc(256);
    return name(buf, 256);
  }

  bool end() const { return end_; }

private:
  char *name(char *buf, size_t n) const {
    n = MIN(DIRSIZ+1, n);
    memmove(buf, de_.name, n-1);
    buf[n-1] = 0;
    return buf;
  } 

  void refill(void) {
    int r;

  again:
    r = read(fd_, &de_, sizeof(de_));
    if (r == sizeof(de_)) {
      if (de_.inum == 0)
        goto again;
    } else {
      end_ = true;
    }
  }

  int fd_;
  bool end_;
  struct dirent de_;
};
