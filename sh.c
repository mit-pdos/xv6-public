#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "fcntl.h"

#define BUFSIZ  512
#define MAXARGS  10
#define MAXNODE 2

// only allocate nodes for i/o redir; at some point we may have to build a 
// a real parse tree.
struct node {
  int token;
  char *s;
};
struct node list[MAXNODE];
int nextnode;

char buf[BUFSIZ];
char *argv[MAXARGS];
char argv0buf[BUFSIZ];
int argc;

int debug = 0;

int parse(char *s);
void runcmd(void);
int ioredirection(void);
int gettoken(char *s, char **token);
int _gettoken(char *s, char **p1, char **p2);
void addnode(int token, char *s);

int
main(void)
{
  while(1){
    puts("$ ");
    memset (buf, '\0', sizeof(buf));
    gets(buf, sizeof(buf));
    if (parse(buf) < 0)
      continue;
    runcmd();
  }
}

int 
parse(char *s)
{
  char *t;
  int c;

  gettoken(s, 0);

  argc = 0;
  nextnode = 0;
  while (1) {
    switch ((c = gettoken(0, &t))) {

    case 'w':	// Add an argument
      if (argc == MAXARGS) {
	printf(2, "too many arguments\n");
	return -1;
      }
      argv[argc++] = t;
      break;
			
    case '<':	// Input redirection
      // Grab the filename from the argument list
      if (gettoken(0, &t) != 'w') {
	printf(2, "syntax error: < not followed by word\n");
	return -1;
      }
      addnode('<', t);
      break;
			
    case '>':	// Output redirection
      // Grab the filename from the argument list
      if (gettoken(0, &t) != 'w') {
	printf(2, "syntax error: > not followed by word\n");
	return -1;
      }
      addnode('>', t);
      break;

    case 0:		// String is complete
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
  int i, r, pid;

  // Return immediately if command line was empty.
  if(argc == 0) {
    if (debug)
      printf(2, "EMPTY COMMAND\n");
    return;
  }

  // Clean up command line.
  // Read all commands from the filesystem: add an initial '/' to
  // the command name.
  // This essentially acts like 'PATH=/'.
  if (argv[0][0] != '/') {
    argv0buf[0] = '/';
    strcpy(argv0buf + 1, argv[0]);
    argv[0] = argv0buf;
  }
  argv[argc] = 0;
	
  // Print the command.
  if (debug) {
    printf(2, "[%d] SPAWN:", getpid());
    for (i = 0; argv[i]; i++)
      printf(2, " %s", argv[i]);
    for (i = 0; i < nextnode; i++) {
      printf(2, "%c %s", list[i].token, list[i].s);
    }
    printf(2, "\n");
  }

  if (strcmp(argv[0], "/cd") == 0) {
    if (debug) printf (2, "/cd %s is build in\n", argv[1]);
    chdir(argv[1]);
    return;
  }

  pid = fork();
  if (pid == 0) {
    if (ioredirection() < 0)
      exit();
    if ((r = exec(argv0buf, (char**) argv)) < 0) {
      printf(2, "exec %s: %d\n", argv[0], r);
      exit();
    }
  }

  if (pid > 0) {
    if (debug)
      printf(2, "[%d] WAIT %s\n", getpid(), argv[0]);
    wait();
    if (debug)
      printf(2, "[%d] wait finished\n", getpid());
  }
}

int
ioredirection(void)
{
  int i, fd, dfd;

  for (i = 0; i < nextnode; i++) {
    switch (list[i].token) {
    case '<':
      if ((fd = open(list[i].s, O_RDONLY)) < 0) {
	printf(2, "failed to open %s for read: %d", list[i].s, fd);
	return -1;
      }

      if (debug)
	printf(2, "redirect 0 from %s\n", list[i].s);

      close(0);
      if ((dfd = dup(fd)) < 0)
	printf(2, "dup failed\n");
      if (debug)
	printf(2, "dup returns %d\n", dfd);
      close(fd);
      break;
    case '>':
      if ((fd = open(list[i].s, O_WRONLY|O_CREATE)) < 0) {
	printf(2, "failed to open %s for write: %d", list[i].s, fd);
	exit();
      }

      if (debug)
	printf(2, "redirect 1 to %s\n", list[i].s);

      if (close(1) < 0)
	printf(2, "close 1 failed\n");
      if ((dfd = dup(fd)) < 0)
	printf(2, "dup failed\n");
      if (debug)
	printf(2, "dup returns %d\n", dfd);
      close(fd);
      break;
    }
  }
  return 0;
}

void
addnode(int token, char *s)
{
  if (nextnode >= MAXNODE) {
    printf(2, "addnode: ran out of nodes\n");
    return;
  }
    
  list[nextnode].token = token;
  list[nextnode].s = s;
  nextnode++;
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
  static char* np1, *np2;

  if (s) {
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
//	0 for end-of-string;
//	< for <;
//	> for >;
//	| for |;
//	w for a word.
//
// Eventually (once we parse the space where the \0 will go),
// words get nul-terminated.
#define WHITESPACE " \t\r\n"
#define SYMBOLS "<|>&;()"

int
_gettoken(char *s, char **p1, char **p2)
{
  int t;

  if (s == 0) {
    if (debug > 1)
      printf(2, "GETTOKEN NULL\n");
    return 0;
  }

  if (debug > 1)
    printf(2, "GETTOKEN: %s\n", s);

  *p1 = 0;
  *p2 = 0;

  while (strchr(WHITESPACE, *s))
    *s++ = 0;
  if (*s == 0) {
    if (debug > 1)
      printf(2, "EOL\n");
    return 0;
  }
  if (strchr(SYMBOLS, *s)) {
    t = *s;
    *p1 = s;
    *s++ = 0;
    *p2 = s;
    if (debug > 1)
      printf(2, "TOK %c\n", t);
    return t;
  }
  *p1 = s;
  while (*s && !strchr(WHITESPACE SYMBOLS, *s))
    s++;
  *p2 = s;
  if (debug > 1) {
    t = **p2;
    **p2 = 0;
    printf(2, "WORD: %s\n", *p1);
    **p2 = t;
  }
  return 'w';
}

