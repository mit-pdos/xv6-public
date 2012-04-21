#include "types.h"
#include "amd64.h"
#include "kernel.hh"
#include "percpu.hh"
#include "wq.hh"
#include "kalloc.hh"
#include "cpputil.hh"

extern "C" void zpage(void*);
extern "C" void zrun_nc(run*);

static const bool prezero = true;

struct zallocator {
  kmem   kmem;
  wframe frame;

  void  init(int);
  char* alloc(const char*);
  void  free(void*);
  void  tryrefill();
};
percpu<zallocator> z_;

struct zwork : public work {
  zwork(wframe* frame, zallocator* zer)
    : frame_(frame), zer_(zer)
  {
    frame_->inc();
  }

  virtual void run() {
    for (int i = 0; i < 32; i++) {
      struct run* r = (struct run*)kalloc("zpage");
      if (r == nullptr)
        break;
      zrun_nc(r);
      zer_->kmem.free(r);
    }
    frame_->dec();
    delete this;
  }

  wframe* frame_;
  zallocator* zer_;

  NEW_DELETE_OPS(zwork);
};

//
// zallocator
//
void
zallocator::tryrefill(void)
{
  if (prezero && kmem.nfree < 16 && frame.zero()) {
    zwork* w = new zwork(&frame, this);
    if (wq_push(w) < 0)
      delete w;
  }
}

void
zallocator::init(int c)
{
  frame.clear();
  kmem.name[0] = (char) c + '0';
  safestrcpy(kmem.name+1, "zmem", MAXNAME-1);
  kmem.size = PGSIZE;
}

char*
zallocator::alloc(const char* name)
{
  char* p;

  p = (char*) kmem.alloc(name);
  if (p == nullptr) {
    p = kalloc(name);
    if (p != nullptr)
      zpage(p);
  } else {
    // Zero the run header used by kmem
    memset(p, 0, sizeof(struct run));
  }
  tryrefill();
  return p;
}

void
zallocator::free(void* p)
{
  if (0) 
    for (int i = 0; i < 4096; i++)
      assert(((char*)p)[i] == 0);

  kmem.free((struct run*)p);
}

char*
zalloc(const char* name)
{
  return z_->alloc(name);
}

void
zfree(void* p)
{
  z_->free(p);
}

void
initz(void)
{
  for (int c = 0; c < NCPU; c++)
    z_[c].init(c);
}
