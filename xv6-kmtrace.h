#include "xv6-mtrace.h"

static inline void mtrace_kstack_register(void *eip,
                                          mtrace_call_state_t state,
                                          int pid)
{
  if (pid == 0)
    pid = proc->pid;
  mtrace_fcall_register(pid, (unsigned long)eip, pid, 0, state);
}
