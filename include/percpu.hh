struct pad {
  __padout__;
} __mpalign__;

template <typename T>
struct percpu {
  int myid() {
    return mycpu()->id;
  }

  const T* operator->() const {
    return cpu(myid());
  }

  T* operator->() {
    return cpu(myid());
  }

  T& operator[](int id) { 
    return *cpu(id);
  }

  const T& operator[](int id) const { 
    return *cpu(id);
  }

  T* cpu(int id) {
    return &v_[id];
  }

  T v_[NCPU] __mpalign__;
};
