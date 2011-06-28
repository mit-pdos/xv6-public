#include "xv6-mtrace.h"

struct kstack_tag {
    int val __attribute__((aligned (CACHELINE)));
};
extern struct kstack_tag kstack_tag[NCPU];

static inline void mtrace_kstack_start(void *eip,
                                       struct proc *p)
{
  unsigned long new_tag;
  int i;

  pushcli();
  xchg((uint *)&i, 0);
  new_tag = ++(kstack_tag[cpu->id].val) | (cpu->id << MTRACE_TAGSHIFT);
  i = ++p->mtrace_stacks.curr;
  if (i >= MTRACE_NSTACKS)
    panic("mtrace_kstack_start: ran out of slots");
  p->mtrace_stacks.tag[i] = new_tag;  
  mtrace_fcall_register(p->pid, (unsigned long)eip,
                        p->mtrace_stacks.tag[i], i, mtrace_start);
  popcli();
}

static inline void mtrace_kstack_stop(struct proc *p)
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

static inline void mtrace_kstack_pause(struct proc *p)
{
  int i;

  i = p->mtrace_stacks.curr;
  if (i < 0)
    panic("mtrace_kstack_pause: bad stack");
  mtrace_fcall_register(p->pid, 0, p->mtrace_stacks.tag[i], i, mtrace_pause);
}

static inline void mtrace_kstack_resume(struct proc *p)
{
  int i;

  i = p->mtrace_stacks.curr;
  if (i < 0)
    panic("mtrace_kstack_resume: bad stack");
  mtrace_fcall_register(p->pid, 0, p->mtrace_stacks.tag[i], i, mtrace_resume);
}
