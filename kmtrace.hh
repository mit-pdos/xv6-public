#include "mtrace.h"
#if MTRACE
// Tell mtrace about switching threads
struct kstack_tag {
  int val __mpalign__;
};
extern struct kstack_tag kstack_tag[NCPU];

// Tell mtrace about switching threads/processes
static inline void mtstart(void *ip, struct proc *p)
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

#else
#define mtstart(ip, p) do { } while (0)
#define mtstop(p) do { } while (0)
#define mtpause(p) do { } while (0)
#define mtresume(p) do { } while (0)
#define mtrec(cpu) do { } while (0)
#define mtign(cpu) do { } while (0)
#define mtrec(cpu) do { } while (0)
#define mtign(cpu) do { } while (0)
#endif
