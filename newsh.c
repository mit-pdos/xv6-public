#include "kernel/fcntl.h"
#include "kernel/param.h"
#include "kernel/types.h"
#include "user/user.h"

#define MAXSIZE 128

void redirect(int d, int pd[]) {
  close(d);
  dup(pd[d]);
  close(pd[0]);
  close(pd[1]);
}

char *splitpipe(char *cmd) {
  for (; *cmd && *cmd != '|'; cmd++)
    ;

  if (*cmd == '|') {
    *cmd++ = '\0';
    return cmd;
  }

  return 0;
}

int isspace(char c) { return c == ' ' || c == '\t' || c == '\n'; }

void execcmd(char *cmd) {
  char *argv[MAXARG];
  int argc = 0, inpos = 0, outpos = 0;

  for (; *cmd; *cmd++ = '\0') {
    if (!isspace(*cmd)) {
      argv[argc++] = cmd;

      if (*cmd == '<')
        inpos = argc;
      if (*cmd == '>')
        outpos = argc;

      for (; *cmd && !isspace(*cmd); cmd++)
        ;
      if (!*cmd) {
        break;
      }
    }
  }

  argv[argc] = 0;

  if (inpos) {
    argv[argc = inpos - 1] = 0;
    close(0);
    open(argv[inpos], O_RDONLY);
  }

  if (outpos) {
    argv[argc = outpos - 1] = 0;
    close(1);
    open(argv[outpos], O_CREATE | O_WRONLY);
  }

  if (argc == 0) {
    exit(0);
  }

  exec(argv[0], argv);

  intf(2, "argv: ");
  for (int i = 0; argv[i]; i++) {
    intf(2, " %s", argv[i]);
  }

  intf(2, "\n");
}

void parsecmd(char *cmd) {
  char *nextcmd = splitpipe(cmd);
  if (!nextcmd) {
    execcmd(cmd);
    intf(2, "exec: %s failed\n", cmd);
    exit(0);
  }

  int pd[2];
  pipe(pd);

  if (fork()) {
    redirect(1, pd);
    execcmd(cmd);
    intf(2, "exec: %s failed\n", cmd);
  } else if (fork()) {
    redirect(0, pd);
    parsecmd(nextcmd);
  }

  close(pd[0]);
  close(pd[1]);
  wait(0);
  wait(0);
  exit(0);
}

int getcmd(char *buf, int nbuf) {
  intf(2, "@ ");
  memset(buf, 0, nbuf);
  gets(buf, nbuf);
  if (buf[0] == 0)
    return -1;
  return 0;
}

int main(void) {
  static char buf[MAXSIZE];

  while (getcmd(buf, sizeof(buf)) == 0) {
    if (fork()) {
      wait(0);
    } else {
      parsecmd(buf);
      intf(2, "parsecmd not exit\n");
    }
  }

  exit(0);
}