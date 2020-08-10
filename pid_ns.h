#define MAX_PID_NS_DEPTH 4

struct pid_ns {
  int ref;
  struct pid_ns* parent;
  struct spinlock lock;
  int next_pid;
  int pid1_ns_killed;  // Indicated whether the process with pid 1 in the namespace was killed or not
};

void pid_ns_init();
void pid_ns_put(struct pid_ns* pid_ns);
void pid_ns_get(struct pid_ns* pid_ns);
int pid_ns_next_pid(struct pid_ns* pid_ns);
struct pid_ns* pid_ns_new(struct pid_ns* parent);
struct pid_ns* pid_ns_dup(struct pid_ns* pid_ns);
int pid_ns_is_max_depth(struct pid_ns* pid_ns);
