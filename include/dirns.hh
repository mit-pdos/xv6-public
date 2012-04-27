#pragma once

u64 namehash(const strbuf<DIRSIZ>&);

#define NINS      251

// Prevent potentially non-scalable bucket groupings
static_assert(NINS != NHASH, "Bad NINS choice");

class dirns : public rcu_freed
{
  typedef xns<strbuf<DIRSIZ>, u32, namehash> ins;

public:
  dirns() : rcu_freed("dirns") {
    for (int i = 0; i < NINS; i++)
      a_[i].store(nullptr);
  }

  ~dirns() {
    for (int i = 0; i < NINS; i++)
      if (a_[i].load() != nullptr) {
        delete a_[i].load();
        a_[i].store(nullptr);
      }
  }

  bool remove(const strbuf<DIRSIZ>& name, const u32* vp = nullptr) {
    u32 i = ah(name);
    if (a_[i].load() != nullptr)
      return a_[i].load()->remove(name, vp);
    return false;
  }

  int insert(const strbuf<DIRSIZ>& name, const u32& inum) {
    ins* ns;
    u32 i;

    i = ah(name);

  retry:
    if (a_[i].load() == nullptr) {
      ns = new ins(false);
      if (!cmpxch(&a_[i], (decltype(ns)) nullptr, ns)) {
        delete ns;
        goto retry;
      }
    } else {
      ns = a_[i];
    }
    
    return ns->insert(name, inum);
  }

  u32 lookup(const strbuf<DIRSIZ>& name) const {
    ins* ns;
    u32 i;

    i = ah(name);
    ns = a_[i].load();
    if (ns != nullptr)
      return ns->lookup(name);
    return 0;
  }

  template<class CB>
  void enumerate(CB cb) const {
    // XXX(sbw) not the same semantics as xns::enumerate.
    // This continues if cb returns true.
    for (int i = 0; i < NINS; i++) {
      ins* ns = a_[i].load();
      if (ns != nullptr)
        ns->enumerate(cb);
    }
  }

  virtual void do_gc(void) {
    delete this;
  }

  NEW_DELETE_OPS(dirns);

private:
  std::atomic<ins*> a_[NINS];

  u32 ah(const strbuf<DIRSIZ>& name) const {
    u64 h = namehash(name);
    return h % NINS;
  }
};
