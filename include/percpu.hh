#pragma once

#include "cpu.hh"
#include "amd64.h"
#include "bits.hh"

extern void pushcli(void);
extern void popcli(void);

// Safety policy for how to protect against CPU migrations while using
// a per-CPU variable.
enum class percpu_safety {
  // Interrupts are disabled so the thread cannot migrate.  This can
  // be done in the calling code, or using the load method to get a
  // scoped cli.
  cli,
  // No protection against migration is required.  The variables are
  // internally thread-safe.  Generally the per-CPU variable is used
  // only as a sharding mechanism.
  internal,
};

template <typename T, percpu_safety S>
struct percpu;

template<typename T>
struct percpuval {
  ~percpuval() {
    if (cli_)
      popcli();
  }

  percpuval(percpuval &&o)
  : val_(o.val_), cli_(o.cli_) {
    o.val_ = nullptr;
    o.cli_ = false;
  }

  percpuval(const percpuval &o) = delete;
  percpuval &operator=(const percpuval &o) = delete;

  T* get() {
    return val_;
  }

  T* operator->() {
    return val_;
  }

  T& operator*() {
    return *val_;
  }

  void release() {
    if (cli_)
      popcli();
    val_ = nullptr;
    cli_ = false;
  }

private:
  constexpr percpuval(T* val, bool cli) : val_(val), cli_(cli) { }
  template<typename, percpu_safety> friend struct percpu;

  T* val_;
  bool cli_;
};

template <typename T, percpu_safety S = percpu_safety::cli>
struct percpu {
  constexpr percpu() = default;

  percpu(const percpu &o) = delete;
  percpu(percpu &&o) = delete;
  percpu &operator=(const percpu &o) = delete;

  // Return an object that wraps this CPU's value and protects against
  // CPU migration using the safety policy as long as it is in scope.
  percpuval<T> load() const {
    if (S == percpu_safety::cli)
      pushcli();
    return percpuval<T>(cpu(myid()), S == percpu_safety::cli);
  }

  // Ignore the safety policy and return the value of this variable
  // for the CPU that is current at some instant between entering and
  // returning from this method.
  T* get_unchecked() const {
    return cpu(myid());
  }

  T* operator->() const {
    if (S == percpu_safety::cli)
      assert(!(readrflags() & FL_IF));
    return cpu(myid());
  }

  T& operator*() const {
    if (S == percpu_safety::cli)
      assert(!(readrflags() & FL_IF));
    return *cpu(myid());
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
