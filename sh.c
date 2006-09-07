#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "fcntl.h"

#define BUFSIZ  512
#define MAXARGS  10
#define MAXIO 2
#define MAXCMD  2

// an embarrassingly naive shell

// some day a real parse tree; for now ad-hoc
struct ionode {
  int token;
  char *s;
};

struct cmd {
  char *argv[MAXARGS];
  char argv0buf[BUFSIZ];
  int argc;
  int token;
  struct ionode iolist[MAXIO];
  struct ionode *io;
};
struct cmd cmdlist[MAXCMD];
struct cmd *cmd;

char buf[BUFSIZ];
int debug = 0;

int parse(char *s);
void runcmd(void);
int ioredirection(struct ionode *iolist, int nio);
int gettoken(char *s, char **token);
int _gettoken(char *s, char **p1, char **p2);

int
main(void)
{
  while(1){
    puts("$ ");
    memset(buf, '\0', sizeof(buf));
    gets(buf, sizeof(buf));
    if(parse(buf) < 0)
      continue;
    runcmd();
  }
}

int
parse(char *s)
{
  char *t;
  int c, i;

  gettoken(s, 0);

  cmd = &cmdlist[0];;
  for(i = 0; i < MAXCMD; i++) {
    cmdlist[i].argc = 0;
    cmdlist[i].token = 0;
    cmdlist[i].io = cmdlist[i].iolist;
  }
  while(1) {
    switch((c = gettoken(0, &t))) {

    case 'w':   // Add an argument
      if(cmd->argc >= MAXARGS) {
        printf(2, "too many arguments\n");
        return -1;
      }
      cmd->argv[cmd->argc++] = t;
      break;

    case '>':   // Input and output redirection
    case '<':
      // Grab the filename from the argument list
      if(gettoken(0, &t) != 'w') {
        printf(2, "syntax error: > not followed by word\n");
        return -1;
      }
      if(cmd->io - cmd->iolist >= MAXIO) {
        printf(2, "too many redirections\n");
        return -1;
      }
      cmd->io->token = c;
      cmd->io->s = t;
      cmd->io++;
      break;

    case ';':  // command sequence
    case '|':  // pipe
      if(cmd->io - cmd->iolist >= MAXIO) {
        printf(2, "too many redirections\n");
        return -1;
      }
      cmd->token = c;
      cmd++;
      break;

    case 0:             // String is complete
      return 0;

    default:
      printf(2, "syntax error: bad return %d from gettoken", c);
      return -1;

    }
  }
}

void
runcmd(void)
{
  int i, r, pid, tfd;
  int fdarray[2];
  struct cmd *c;
  struct ionode *io;

  // Return immediately if command line was empty.
  if(cmdlist[0].argc == 0) {
    if(debug)
      printf(2, "EMPTY COMMAND\n");
    return;
  }

  for(c = &cmdlist[0]; c <= cmd; c++) {
    // Clean up command line.
    // Read all commands from the filesystem: add an initial '/' to
    // the command name.
    // This essentially acts like 'PATH=/'.
    if(c->argv[0][0] != '/') {
      c->argv0buf[0] = '/';
      strcpy(c->argv0buf + 1, c->argv[0]);
      c->argv[0] = c->argv0buf;
    }
    c->argv[c->argc] = 0;

    // Print the command.
    if(debug) {
      printf(2, "[%d] SPAWN:", getpid());
      for(i = 0; c->argv[i]; i++)
        printf(2, " %s", c->argv[i]);
      for(io = c->iolist; io <= c->io; io++) {
        printf(2, "%c %s", io->token, io->s);
      }
      printf(2, "\n");
    }

    if(strcmp(c->argv[0], "/cd") == 0) {
      if(debug)
        printf (2, "/cd %s is build in\n", c->argv[1]);
      chdir(c->argv[1]);
      return;
    }

    if(c->token == '|')
      if(pipe(fdarray) < 0)
        printf(2, "cmd %d pipe failed\n", c);

    pid = fork();
    if(pid == 0) {
      if(c->token == '|') {
        if(close(1) < 0)
          printf(2, "close 1 failed\n");
        if((tfd = dup(fdarray[1])) < 0)
          printf(2, "dup failed\n");
        if(close(fdarray[0]) < 0)
          printf(2, "close fdarray[0] failed\n");
        if(close(fdarray[1]) < 0)
          printf(2, "close fdarray[1] failed\n");
      }
      if(c > cmdlist && (c-1)->token == '|') {
        if(close(0) < 0)
          printf(2, "close 0 failed\n");
        if((tfd = dup(fdarray[0])) < 0)
          printf(2, "dup failed\n");
        if(close(fdarray[0]) < 0)
          printf(2, "close fdarray[0] failed\n");
        if(close(fdarray[1]) < 0)
          printf(2, "close fdarray[1] failed\n");
      }
      if(ioredirection(c->iolist, c->io - c->iolist) < 0)
        exit();
      if((r = exec(c->argv0buf, (char**) c->argv)) < 0) {
        printf(2, "exec %s: %d\n", c->argv[0], r);
        exit();
      }
    } else if(pid > 0) {
      int p;
      if(debug)
        printf(2, "[%d] FORKED child %d\n", getpid(), pid);

      if(c > cmdlist && (c-1)->token == '|') {
        close(fdarray[0]);
        close(fdarray[1]);
      }
      if(c->token != '|') {
        if(debug)
          printf(2, "[%d] WAIT for children\n", getpid());
        do {
          p = wait();
          if(debug)
            printf(2, "[%d] WAIT child %d finished\n", getpid(), p);
        } while(p > 0);
        if(debug)
          printf(2, "[%d] wait finished\n", getpid());
      }
    }
  }
}

int
ioredirection(struct ionode *iolist, int nio)
{
  int fd;
  struct ionode *io;

  for(io = iolist; io < &iolist[nio]; io++) {
    switch(io->token) {
    case '<':
      if(close(0) < 0)
        printf(2, "close 0 failed\n");
      if((fd = open(io->s, O_RDONLY)) < 0) {
        printf(2, "failed to open %s for read: %d", io->s, fd);
        return -1;
      }
      if(debug)
        printf(2, "redirect 0 from %s\n", io->s);
      break;
    case '>':
      if(close(1) < 0)
        printf(2, "close 1 failed\n");
      if((fd = open(io->s, O_WRONLY|O_CREATE)) < 0) {
        printf(2, "failed to open %s for write: %d", io->s, fd);
        exit();
      }
      if(debug)
        printf(2, "redirect 1 to %s\n", io->s);
      break;
    }
  }
  return 0;
}

// gettoken(s, 0) prepares gettoken for subsequent calls and returns 0.
// gettoken(0, token) parses a shell token from the previously set string,
// null-terminates that token, stores the token pointer in '*token',
// and returns a token ID (0, '<', '>', '|', or 'w').
// Subsequent calls to 'gettoken(0, token)' will return subsequent
// tokens from the string.

int
gettoken(char *s, char **p1)
{
  static int c, nc;
  static char *np1, *np2;

  if(s) {
    nc = _gettoken(s, &np1, &np2);
    return 0;
  }
  c = nc;
  *p1 = np1;
  nc = _gettoken(np2, &np1, &np2);
  return c;
}


// Get the next token from string s.
// Set *p1 to the beginning of the token and *p2 just past the token.
// Returns
//      0 for end-of-string;
//      < for <;
//      > for >;
//      | for |;
//      w for a word.
//
// Eventually (once we parse the space where the \0 will go),
// words get nul-terminated.
#define WHITESPACE " \t\r\n"
#define SYMBOLS "<|>&;()"

int
_gettoken(char *s, char **p1, char **p2)
{
  int t;

  if(s == 0) {
    if(debug > 1)
      printf(2, "GETTOKEN 0\n");
    return 0;
  }

  if(debug > 1)
    printf(2, "GETTOKEN: %s\n", s);

  *p1 = 0;
  *p2 = 0;

  while(strchr(WHITESPACE, *s))
    *s++ = 0;
  if(*s == 0) {
    if(debug > 1)
      printf(2, "EOL\n");
    return 0;
  }
  if(strchr(SYMBOLS, *s)) {
    t = *s;
    *p1 = s;
    *s++ = 0;
    *p2 = s;
    if(debug > 1)
      printf(2, "TOK %c\n", t);
    return t;
  }
  *p1 = s;
  while(*s && !strchr(WHITESPACE SYMBOLS, *s))
    s++;
  *p2 = s;
  if(debug > 1) {
    t = **p2;
    **p2 = 0;
    printf(2, "WORD: %s\n", *p1);
    **p2 = t;
  }
  return 'w';
}

