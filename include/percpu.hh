#pragma once

extern int mycpuid(void);

template <typename T>
struct percpu {
  percpu() = default;

  percpu(const percpu &o) = delete;
  percpu(percpu &&o) = delete;
  percpu &operator=(const percpu &o) = delete;

  T* get() const {
    return cpu(mycpuid());
  }

  T* operator->() const {
    return cpu(mycpuid());
  }

  T& operator*() const {
    return *cpu(mycpuid());
  }

  T& operator[](int id) const {
    return *cpu(id);
  }

private:
  T* cpu(int id) const {
    return &pad_[id].v_;
  }

  // percpu acts like a T* const, but since it's actually storing the
  // data directly, we have to strip the const-ness away from the data
  // being stored.  This lets const members return non-const pointers
  // to this data, just like a T* const.
  mutable struct {
    T v_ __mpalign__;
    __padout__;
  } pad_[NCPU];
};
