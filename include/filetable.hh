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
        ofile_[fd]->dec();
        ofile_[fd] = 0;
      }
    }
    destroylock(&lock_);
  }

  bool getfile(int fd, sref<file> *sf) {
    if (fd < 0 || fd >= NOFILE)
      return false;

    scoped_gc_epoch gc;
    file* f = ofile_[fd];
    if (!f || !sf->init(f))
      return false;
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
    cprintf("filetable::allocfd: failed\n");
    return -1;
  }

  void close(int fd) {
    // XXX(sbw) if f->ref_ > 1 the kernel will not actually close 
    // the file when this function returns (i.e. sys_close can return 
    // while the file/pipe/socket is still open).  Maybe we should clear 
    // ofile_[fd], wait until f.ref_ == 1, f->dec(), and then return.
    
    acquire(&lock_);
    struct file *f = ofile_[fd];
    ofile_[fd] = nullptr;
    release(&lock_);
    if (f)
      f->dec();
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
