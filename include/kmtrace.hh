#pragma once

#include "mtrace.h"

#if MTRACE
// Tell mtrace about switching threads
struct kstack_tag {
  int val __mpalign__;
};
extern struct kstack_tag kstack_tag[NCPU];

// Tell mtrace about switching threads/processes
template<typename Ret, typename... Args>
static inline void mtstart(Ret (*ip)(Args...), struct proc *p)
{
  unsigned long new_tag;
  int i;

  pushcli();
  new_tag = ++(kstack_tag[mycpu()->id].val) | (mycpu()->id<<MTRACE_TAGSHIFT);
  i = ++p->mtrace_stacks.curr;
  if (i >= MTRACE_NSTACKS)
    panic("mtrace_kstack_start: ran out of slots");
  p->mtrace_stacks.tag[i] = new_tag;  
  mtrace_fcall_register(p->pid, (unsigned long)ip,
                        p->mtrace_stacks.tag[i], i, mtrace_start);
  popcli();
}

static inline void mtstop(struct proc *p)
{
  int i;
  pushcli();
  i = p->mtrace_stacks.curr;
  if (i < 0)
    panic("mtrace_kstack_stop: fell off of stack");
  mtrace_fcall_register(p->pid, 0, p->mtrace_stacks.tag[i], i, mtrace_done);
  p->mtrace_stacks.tag[i] = 0;
  p->mtrace_stacks.curr--;
  popcli();
}

static inline void mtpause(struct proc *p)
{
  int i;

  i = p->mtrace_stacks.curr;
  if (i < 0)
    panic("mtrace_kstack_pause: bad stack");
  mtrace_fcall_register(p->pid, 0, p->mtrace_stacks.tag[i], i, mtrace_pause);
}

static inline void mtresume(struct proc *p)
{
  int i;

  i = p->mtrace_stacks.curr;
  if (i < 0)
    panic("mtrace_kstack_resume: bad stack");
  mtrace_fcall_register(p->pid, 0, p->mtrace_stacks.tag[i], i, mtrace_resume);
}

// Tell mtrace to start/stop recording call and ret
#define mtrec() mtrace_call_set(1, ~0ull)
#define mtign() mtrace_call_set(0, ~0ull)

class mt_ascope
{
  char name[64];
  bool active;
public:
  explicit mt_ascope(const char *fmt, ...)
  {
    va_list ap;

    va_start(ap, fmt);
    vopen(fmt, ap);
    va_end(ap);
  }

  ~mt_ascope()
  {
    close();
  }

  void open(const char *fmt, ...)
  {
    va_list ap;

    va_start(ap, fmt);
    vopen(fmt, ap);
    va_end(ap);
  }

  void vopen(const char *fmt, va_list ap)
  {
    vsnprintf(name, sizeof(name) - 1, fmt, ap);
    mtrace_ascope_register(0, name);
    active = true;
  }

  void close()
  {
    if (active)
      mtrace_ascope_register(1, name);
    active = false;
  }
};

static inline void mtreadavar(const char *fmt, ...)
{
  char name[64];
  va_list ap;

  va_start(ap, fmt);
  vsnprintf(name, sizeof(name), fmt, ap);
  va_end(ap);

  mtrace_avar_register(0, name);
}

static inline void mtwriteavar(const char *fmt, ...)
{
  char name[64];
  va_list ap;

  va_start(ap, fmt);
  vsnprintf(name, sizeof(name), fmt, ap);
  va_end(ap);

  mtrace_avar_register(1, name);
}

#else
#define mtstart(ip, p) do { } while (0)
#define mtstop(p) do { } while (0)
#define mtpause(p) do { } while (0)
#define mtresume(p) do { } while (0)
#define mtrec(cpu) do { } while (0)
#define mtign(cpu) do { } while (0)
#define mtrec(cpu) do { } while (0)
#define mtign(cpu) do { } while (0)

class mt_ascope
{
public:
  explicit mt_ascope(const char *fmt, ...) {}
  void open(const char *fmt, ...) {}
  void close() {}
};
#define mtreadavar(fmt, ...) do { } while (0)
#define mtwriteavar(fmt, ...) do { } while (0)

#endif
