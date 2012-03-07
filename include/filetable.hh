#include "atomic.hh"

class filetable {
public:
  filetable() : ref_(1) {
    for(int fd = 0; fd < NOFILE; fd++)
      ofile_[fd] = nullptr;
    initlock(&lock_, "filetable", 0);
  }

  filetable(const filetable &f) : ref_(1) {
    for(int fd = 0; fd < NOFILE; fd++) {
      if (f.ofile_[fd])
        ofile_[fd] = f.ofile_[fd]->dup();
      else
        ofile_[fd] = nullptr;
    }
    initlock(&lock_, "filetable", 0);
  }
  
  ~filetable() {
    for(int fd = 0; fd < NOFILE; fd++){
      if (ofile_[fd]){
        ofile_[fd]->close();
        ofile_[fd] = 0;
      }
    }
    destroylock(&lock_);
  }

  bool getfile(int fd, sref<file> *sf) {
    file *f;

    if (fd < 0 || fd >= NOFILE)
      return false;
    acquire(&lock_);
    f = ofile_[fd];
    if (!f) {
      release(&lock_);
      return false;
    }
    sf->init(f);
    release(&lock_);
    return true;
  }

  int allocfd(struct file *f) {
    acquire(&lock_);
    for (int fd = 0; fd < NOFILE; fd++) {
      if (ofile_[fd] == nullptr){
        ofile_[fd] = f;
        release(&lock_);
        return fd;
      }
    }
    release(&lock_);    
    return -1;
  }

  void close(int fd) {
    struct file *f = ofile_[fd];
    
    acquire(&lock_);
    ofile_[fd] = nullptr;
    release(&lock_);
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
  struct spinlock lock_;
};
