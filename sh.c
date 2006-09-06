#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "fcntl.h"

#define BUFSIZ  512
#define MAXARGS  10
#define MAXNODE 2
#define MAXCMD  2

// an embarrassingly naive shell

// some day a real parse tree; for now ad-hoc
struct ionode {
  int token;
  char *s;
};
struct ionode iolist[MAXNODE];
int nextio;

struct cmd {
  char *argv[MAXARGS];
  char argv0buf[BUFSIZ];
  int argc;
  int token;
};
struct cmd cmdlist[MAXCMD];
int nextcmd;

char buf[BUFSIZ];
int debug = 0;

int parse(char *s);
void runcmd(void);
int ioredirection(void);
int gettoken(char *s, char **token);
int _gettoken(char *s, char **p1, char **p2);
void addio(int token, char *s);

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

  nextio = 0;
  nextcmd = 0;
  for(i = 0; i < MAXCMD; i++) {
    cmdlist[i].argc = 0;
    cmdlist[i].token = 0;
  }
  while(1) {
    switch((c = gettoken(0, &t))) {

    case 'w':   // Add an argument
      if(cmdlist[nextcmd].argc >= MAXARGS) {
        printf(2, "too many arguments\n");
        return -1;
      }
      cmdlist[nextcmd].argv[cmdlist[nextcmd].argc++] = t;
      break;

    case '<':   // Input redirection
      // Grab the filename from the argument list
      if(gettoken(0, &t) != 'w') {
        printf(2, "syntax error: < not followed by word\n");
        return -1;
      }
      addio('<', t);
      break;

    case '>':   // Output redirection
      // Grab the filename from the argument list
      if(gettoken(0, &t) != 'w') {
        printf(2, "syntax error: > not followed by word\n");
        return -1;
      }
      addio('>', t);
      break;

    case ';':  // command sequence
    case '|':  // pipe
      cmdlist[nextcmd].token = c;
      nextcmd++;
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
  int c, i, r, pid, tfd;
  int fdarray[2];

  // Return immediately if command line was empty.
  if(cmdlist[0].argc == 0) {
    if(debug)
      printf(2, "EMPTY COMMAND\n");
    return;
  }

  for(c = 0; c <= nextcmd; c++) {
    // Clean up command line.
    // Read all commands from the filesystem: add an initial '/' to
    // the command name.
    // This essentially acts like 'PATH=/'.
    if(cmdlist[c].argv[0][0] != '/') {
      cmdlist[c].argv0buf[0] = '/';
      strcpy(cmdlist[c].argv0buf + 1, cmdlist[c].argv[0]);
      cmdlist[c].argv[0] = cmdlist[c].argv0buf;
    }
    cmdlist[c].argv[cmdlist[c].argc] = 0;

    // Print the command.
    if(debug) {
      printf(2, "[%d] SPAWN:", getpid());
      for(i = 0; cmdlist[c].argv[i]; i++)
        printf(2, " %s", cmdlist[c].argv[i]);
      for(i = 0; i < nextio; i++) {
        printf(2, "%c %s", iolist[i].token, iolist[i].s);
      }
      printf(2, "\n");
    }

    if(strcmp(cmdlist[c].argv[0], "/cd") == 0) {
      if(debug)
        printf (2, "/cd %s is build in\n", cmdlist[c].argv[1]);
      chdir(cmdlist[c].argv[1]);
      return;
    }

    if(cmdlist[c].token == '|')
      if(pipe(fdarray) < 0)
        printf(2, "cmd %d pipe failed\n", c);

    pid = fork();
    if(pid == 0) {
      if(cmdlist[c].token == '|') {
        if(close(1) < 0)
          printf(2, "close 1 failed\n");
        if((tfd = dup(fdarray[1])) < 0)
          printf(2, "dup failed\n");
        if(close(fdarray[0]) < 0)
          printf(2, "close fdarray[0] failed\n");
        if(close(fdarray[1]) < 0)
          printf(2, "close fdarray[1] failed\n");
      }
      if(c > 0 && cmdlist[c-1].token == '|') {
        if(close(0) < 0)
          printf(2, "close 0 failed\n");
        if((tfd = dup(fdarray[0])) < 0)
          printf(2, "dup failed\n");
        if(close(fdarray[0]) < 0)
          printf(2, "close fdarray[0] failed\n");
        if(close(fdarray[1]) < 0)
          printf(2, "close fdarray[1] failed\n");
      }
      if(ioredirection() < 0)
        exit();
      if((r = exec(cmdlist[c].argv0buf, (char**) cmdlist[c].argv)) < 0) {
        printf(2, "exec %s: %d\n", cmdlist[c].argv[0], r);
        exit();
      }
    } else if(pid > 0) {
      int p;
      if(debug)
        printf(2, "[%d] FORKED child %d\n", getpid(), pid);

      if(c > 0 && cmdlist[c-1].token == '|') {
        close(fdarray[0]);
        close(fdarray[1]);
      }
      if(cmdlist[c].token != '|') {
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
ioredirection(void)
{
  int i, fd;

  for(i = 0; i < nextio; i++) {
    switch(iolist[i].token) {
    case '<':
      if(close(0) < 0)
        printf(2, "close 0 failed\n");
      if((fd = open(iolist[i].s, O_RDONLY)) < 0) {
        printf(2, "failed to open %s for read: %d", iolist[i].s, fd);
        return -1;
      }
      if(debug)
        printf(2, "redirect 0 from %s\n", iolist[i].s);
      break;
    case '>':
      if(close(1) < 0)
        printf(2, "close 1 failed\n");
      if((fd = open(iolist[i].s, O_WRONLY|O_CREATE)) < 0) {
        printf(2, "failed to open %s for write: %d", iolist[i].s, fd);
        exit();
      }
      if(debug)
        printf(2, "redirect 1 to %s\n", iolist[i].s);
      break;
    }
  }
  return 0;
}

void
addio(int token, char *s)
{
  if(nextio >= MAXNODE) {
    printf(2, "addio: ran out of nodes\n");
    return;
  }

  iolist[nextio].token = token;
  iolist[nextio].s = s;
  nextio++;
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

