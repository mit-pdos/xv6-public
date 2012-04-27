#pragma once

u64 namehash(const strbuf<DIRSIZ>&);

class dirns : public rcu_freed
{
public:
  dirns() : rcu_freed("dirns"), ns_(false) {}

  bool remove(const strbuf<DIRSIZ>& name, const u32* vp = nullptr) {
    return ns_.remove(name, vp);
  }

  int  insert(const strbuf<DIRSIZ>& name, const u32& inum) {
    return ns_.insert(name, inum);
  }

  u32  lookup(const strbuf<DIRSIZ>& name) {
    return ns_.lookup(name);
  }

  template<class CB>
  void enumerate(CB cb) {
    return ns_.enumerate(cb);
  }

  virtual void do_gc(void) {
    delete this;
  }

  NEW_DELETE_OPS(dirns);

private:
  xns<strbuf<DIRSIZ>, u32, namehash> ns_;
};
