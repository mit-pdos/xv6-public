#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "x86.h"
#include "proc.h"
#include "spinlock.h"
#include "wstatus.h"
#include "pid_ns.h"
#include "namespace.h"

struct {
  struct spinlock lock;
  struct pid_ns namespaces[NNAMESPACE];
} pidnstable;

void pid_ns_init()
{
  initlock(&pidnstable.lock, "pidns");
  for (int i = 0; i < NNAMESPACE; i++) {
    initlock(&pidnstable.namespaces[i].lock, "pidns");
    pidnstable.namespaces[i].ref = 0;
  }
}

void pid_ns_put(struct pid_ns* pid_ns)
{
  acquire(&pidnstable.lock);
  if (!pid_ns->ref) {
      panic("pid_ns_put: ref == 0");
  }
  pid_ns->ref--;
  release(&pidnstable.lock);
}

void pid_ns_get(struct pid_ns* pid_ns)
{
  acquire(&pidnstable.lock);
  pid_ns->ref++;
  release(&pidnstable.lock);
}

struct pid_ns* pid_ns_alloc() {
  acquire(&pidnstable.lock);
  for (int i = 0; i < NNAMESPACE; i++) {
    struct pid_ns* pid_ns = &pidnstable.namespaces[i];
    if (pid_ns->ref == 0) {
      pid_ns->ref = 1;
      release(&pidnstable.lock);
      return pid_ns;
    }
  }
  release(&pidnstable.lock);
  panic("out of pid_ns objects");
}

void pid_ns_init_ns(struct pid_ns* pid_ns, struct pid_ns* parent) {
  pid_ns->parent = parent;
  pid_ns->next_pid = 1;
  pid_ns->pid1_ns_killed = 0;
}
struct pid_ns* pid_ns_dup(struct pid_ns* pid_ns) {
  pid_ns_get(pid_ns);
  return pid_ns;
}

struct pid_ns* pid_ns_new(struct pid_ns* parent) {
  struct pid_ns * pid_ns = pid_ns_alloc();
  pid_ns_init_ns(pid_ns, parent);
  return pid_ns;
}

int pid_ns_next_pid(struct pid_ns* pid_ns) {
  acquire(&pid_ns->lock);
  int pid = pid_ns->next_pid++;
  release(&pid_ns->lock);
  return pid;
}

int pid_ns_is_max_depth(struct pid_ns* pid_ns) {
  int depth = 0;
  while (pid_ns) {
    depth++;
    pid_ns = pid_ns->parent;
  }
  return depth >= MAX_PID_NS_DEPTH;
}
