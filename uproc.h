struct uproc {
  int pid;
  int ppid;
  int state;
  uint sz;
  int uptime;
  int ticks;
  int sleepticks;
  char name[16];
};
