#include "atomic.hh"

class filetable {
public:
  filetable() : ref_(1) {
    for(int fd = 0; fd < NOFILE; fd++)
      ofile_[fd] = nullptr;
  }

  filetable(const filetable &f) : ref_(1) {
    for(int fd = 0; fd < NOFILE; fd++) {
      if (f.ofile_[fd])
        ofile_[fd] = f.ofile_[fd]->dup();
      else
        ofile_[fd] = nullptr;
    }
  }
  
  ~filetable() {
    for(int fd = 0; fd < NOFILE; fd++){
      if (ofile_[fd]){
        ofile_[fd]->close();
        ofile_[fd] = 0;
      }
    }
  }

  struct file *getfile(int fd) {
    if (fd < 0 || fd >= NOFILE)
      return nullptr;

    return ofile_[fd];
  }

  int allocfd(struct file *f) {
    for (int fd = 0; fd < NOFILE; fd++){
      if (ofile_[fd] == nullptr){
        ofile_[fd] = f;
        return fd;
      }
    }
    
    return -1;
  }

  void close(int fd) {
    struct file *f = ofile_[fd];
    ofile_[fd] = nullptr;
    f->close();
  }

  void decref() {
    if (--ref_ == 0)
      delete this;
  }

  void incref() {
    ref_++;
  }

  NEW_DELETE_OPS(filetable)

private:
  struct file *ofile_[NOFILE];
  std::atomic<u64> ref_;
};
