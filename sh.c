// Shell.

#include "types.h"
#include "user.h"
#include "fcntl.h"
#include "wstatus.h"

// Parsed command representation
#define EXEC  1
#define REDIR 2
#define PIPE  3
#define LIST  4
#define BACK  5

// Parsed internal commands
enum internal_cmd {
    INTERNAL_CMD_NONE,
    INTERNAL_CMD_DOLLAR_QUESTION,
    INTERNAL_CMD_CD,
    INTERNAL_CMD_EXIT,
    INTERNAL_CMD_CONNECT_TTY,
    INTERNAL_CMD_DISCONNECT_TTY,
    INTERNAL_CMD_ATTACH_TTY,
    INTERNAL_CMD_PID
};


#define MAXARGS 10

// Internal command ran successfully
#define RUN_INTERNAL_CMD_OK 0
// It is not an internal command
#define RUN_INTERNAL_CMD_NONE -1
// Running internal command resulted in error
#define RUN_INTERNAL_CMD_ERR -2


struct cmd {
  int type;
};

struct execcmd {
  int type;
  char *argv[MAXARGS];
  char *eargv[MAXARGS];
};

struct redircmd {
  int type;
  struct cmd *cmd;
  char *file;
  char *efile;
  int mode;
  int fd;
};

struct pipecmd {
  int type;
  struct cmd *left;
  struct cmd *right;
};

struct listcmd {
  int type;
  struct cmd *left;
  struct cmd *right;
};

struct backcmd {
  int type;
  struct cmd *cmd;
};

int fork1(void);  // Fork but panics on failure.
void panic(char*);
struct cmd *parsecmd(char*);
static int last_cmd_retval; // keeps the value of last cmd's return value


static void last_cmd_retval_no_error(void)
{
  last_cmd_retval = 0;
}

static void last_cmd_retval_set_error(int err)
{
  last_cmd_retval = err;
}

static void last_cmd_retval_update_wait_exit_status(void)
{
  last_cmd_retval = WEXITSTATUS(last_cmd_retval);
}

// Execute cmd.  Never returns.
void
runcmd(struct cmd *cmd)
{
  int p[2];
  int fd;
  struct backcmd *bcmd;
  struct execcmd *ecmd;
  struct listcmd *lcmd;
  struct pipecmd *pcmd;
  struct redircmd *rcmd;

  if(cmd == 0){
      exit(1);
    }



  switch(cmd->type){
  default:
    panic("runcmd");

  case EXEC:
    ecmd = (struct execcmd*)cmd;
    if(ecmd->argv[0] == 0){
      exit(1);
    }

    exec(ecmd->argv[0], ecmd->argv);

    fd = open(ecmd->argv[0], O_RDONLY);
    if (-1 != fd) {
        close(fd);
        fd = -1;
    } else {
        char * root_cmd = malloc(strlen(ecmd->argv[0]) + 1);
        root_cmd[0] = '/';
        strcpy(root_cmd + 1, ecmd->argv[0]);
        exec(root_cmd, ecmd->argv);
        free(root_cmd);
    }

    printf(2, "exec %s failed\n", ecmd->argv[0]);
    exit(1);
    break;

  case REDIR:
    rcmd = (struct redircmd*)cmd;
    close(rcmd->fd);
    if(open(rcmd->file, rcmd->mode) < 0){
      printf(2, "open %s failed\n", rcmd->file);
      exit(1);
    }
    runcmd(rcmd->cmd);
    break;

  case LIST:
    lcmd = (struct listcmd*)cmd;
    if(fork1() == 0)
      runcmd(lcmd->left);
    wait(0);
    runcmd(lcmd->right);
    break;

  case PIPE:
    pcmd = (struct pipecmd*)cmd;
    if(pipe(p) < 0)
      panic("pipe");
    if(fork1() == 0){
      close(1);
      dup(p[1]);
      close(p[0]);
      close(p[1]);
      runcmd(pcmd->left);
    }
    if(fork1() == 0){
      close(0);
      dup(p[0]);
      close(p[0]);
      close(p[1]);
      runcmd(pcmd->right);
    }
    close(p[0]);
    close(p[1]);
    wait(0);
    wait(0);
    break;

  case BACK:
    bcmd = (struct backcmd*)cmd;
    if(fork1() == 0)
      runcmd(bcmd->cmd);
    break;
  }

  exit(0);
}

int
getcmd(char *buf, int nbuf)
{
  printf(2, "$ ");
  memset(buf, 0, nbuf);
  gets(buf, nbuf);
  if(buf[0] == 0) // EOF
    return -1;
  return 0;
}

int get_internal_cmd_type(struct execcmd* cmd)
{
    int cmd_type = INTERNAL_CMD_NONE;

    if ((strcmp(cmd->argv[0], "echo") == 0) && (strcmp(cmd->argv[1], "$?") == 0)) {
        cmd_type = INTERNAL_CMD_DOLLAR_QUESTION;
    }
    else if (strcmp(cmd->argv[0], "cd") == 0) {
        cmd_type = INTERNAL_CMD_CD;
    }
    else if (strcmp(cmd->argv[0], "exit") == 0) {
        cmd_type = INTERNAL_CMD_EXIT;
    }
    else if ((strcmp(cmd->argv[0], "connect") == 0) && (strcmp(cmd->argv[1], "tty") == 0)) {
        cmd_type = INTERNAL_CMD_CONNECT_TTY;
    }
    else if (strcmp(cmd->argv[0], "disconnect") == 0) {
        cmd_type = INTERNAL_CMD_DISCONNECT_TTY;
    }
    else if ((strcmp(cmd->argv[0], "attach") == 0) && (strcmp(cmd->argv[1], "tty") == 0)) {
        cmd_type = INTERNAL_CMD_ATTACH_TTY;
    }
    else if (strcmp(cmd->argv[0], "pid") == 0) {
        cmd_type = INTERNAL_CMD_PID;
    }

    return cmd_type;
}


// Executes internal commands
// Returns
// RUN_INTERNAL_CMD_OK : Successful internal command run
// RUN_INTERNAL_CMD_NONE : No internal command ran
// RUN_INTERNAL_CMD_ERROR : There was an error during execution of internal command
int
runinternal(struct cmd** pcmd) {
    struct execcmd* cmd = (struct execcmd*)*pcmd;

    int cmd_type = get_internal_cmd_type(cmd);
    int tty_fd;
    switch (cmd_type)
    {
    case INTERNAL_CMD_DOLLAR_QUESTION:
      printf(1, "%d\n", last_cmd_retval);
      return RUN_INTERNAL_CMD_OK;
      break;

    case INTERNAL_CMD_CD:
      // Chdir must be called by the parent, not the child.
      if (chdir(cmd->argv[1]) < 0) {
        printf(2, "cannot cd %s\n", cmd->argv[1]);
        last_cmd_retval_set_error(RUN_INTERNAL_CMD_ERR);
        return RUN_INTERNAL_CMD_ERR;
      }
      return RUN_INTERNAL_CMD_OK;
      break;

    case INTERNAL_CMD_EXIT:
      // exit must be called by the parent, not the child.
      disconnect_tty(0);
      exit(0);
      break;

    case INTERNAL_CMD_CONNECT_TTY:

      tty_fd = open(cmd->argv[2], O_RDWR);
      if(tty_fd < 0){
        printf(2, "exec connect tty failed\n");
        last_cmd_retval_set_error(RUN_INTERNAL_CMD_ERR);
        return RUN_INTERNAL_CMD_ERR;
      }

      if(connect_tty(tty_fd) < 0){
        close(tty_fd);
        last_cmd_retval_set_error(RUN_INTERNAL_CMD_ERR);
        return RUN_INTERNAL_CMD_ERR;
      }

      close(tty_fd);
      return RUN_INTERNAL_CMD_OK;
      break;

    case INTERNAL_CMD_DISCONNECT_TTY:
    
      if (disconnect_tty(0) != 0)
          printf(2, "disconnect tty failed\n");
      sleep(100);
      return RUN_INTERNAL_CMD_OK;
      break;

    case INTERNAL_CMD_ATTACH_TTY:

      tty_fd = open(cmd->argv[2], O_RDWR);
      if(tty_fd < 0){
        printf(2, "exec attach tty failed\n");
        last_cmd_retval_set_error(RUN_INTERNAL_CMD_ERR);
        return RUN_INTERNAL_CMD_ERR;
      }

      if(attach_tty(tty_fd) < 0){
        printf(2, "exec attach tty failed 2\n");
        close(tty_fd);
        return RUN_INTERNAL_CMD_OK;
      }

      ioctl(tty_fd, TTYSETS, DEV_CONNECT);
      close(tty_fd);
      printf(2, "%s attached\n", cmd->argv[2]);
      return RUN_INTERNAL_CMD_OK;
      break;

    case INTERNAL_CMD_PID:
      printf(2, "PID: %d\n", getpid());
      return RUN_INTERNAL_CMD_OK;
      break;

    default:
      return RUN_INTERNAL_CMD_NONE;
      break;
    }

}

int
main(void)
{
  static char buf[100];
  int fd, retval;
  struct cmd* pcmd;

  // Ensure that three file descriptors are open.
  while((fd = open("console", O_RDWR)) >= 0){
    if(fd >= 3){
      close(fd);
      break;
    }
  }

  // Read and run input commands.
  while(getcmd(buf, sizeof(buf)) >= 0){
      pcmd = parsecmd(buf);
      retval = runinternal(&pcmd);
      if(retval == 0) {
      last_cmd_retval_no_error();
      continue;
    } else if (retval == -2) {
      continue;
    }

    if(fork1() == 0)
      runcmd(pcmd);
    wait(&last_cmd_retval);
    last_cmd_retval_update_wait_exit_status();
  }
  exit(0);
}

void
panic(char *s)
{
  printf(2, "%s\n", s);
  exit(1);
}

int
fork1(void)
{
  int pid;

  pid = fork();
  if(pid == -1)
    panic("fork");
  return pid;
}

//PAGEBREAK!
// Constructors

struct cmd*
execcmd(void)
{
  struct execcmd *cmd;

  cmd = malloc(sizeof(*cmd));
  memset(cmd, 0, sizeof(*cmd));
  cmd->type = EXEC;
  return (struct cmd*)cmd;
}

struct cmd*
redircmd(struct cmd *subcmd, char *file, char *efile, int mode, int fd)
{
  struct redircmd *cmd;

  cmd = malloc(sizeof(*cmd));
  memset(cmd, 0, sizeof(*cmd));
  cmd->type = REDIR;
  cmd->cmd = subcmd;
  cmd->file = file;
  cmd->efile = efile;
  cmd->mode = mode;
  cmd->fd = fd;
  return (struct cmd*)cmd;
}

struct cmd*
pipecmd(struct cmd *left, struct cmd *right)
{
  struct pipecmd *cmd;

  cmd = malloc(sizeof(*cmd));
  memset(cmd, 0, sizeof(*cmd));
  cmd->type = PIPE;
  cmd->left = left;
  cmd->right = right;
  return (struct cmd*)cmd;
}

struct cmd*
listcmd(struct cmd *left, struct cmd *right)
{
  struct listcmd *cmd;

  cmd = malloc(sizeof(*cmd));
  memset(cmd, 0, sizeof(*cmd));
  cmd->type = LIST;
  cmd->left = left;
  cmd->right = right;
  return (struct cmd*)cmd;
}

struct cmd*
backcmd(struct cmd *subcmd)
{
  struct backcmd *cmd;

  cmd = malloc(sizeof(*cmd));
  memset(cmd, 0, sizeof(*cmd));
  cmd->type = BACK;
  cmd->cmd = subcmd;
  return (struct cmd*)cmd;
}
//PAGEBREAK!
// Parsing

char whitespace[] = " \t\r\n\v";
char symbols[] = "<|>&;()";

int
gettoken(char **ps, char *es, char **q, char **eq)
{
  char *s;
  int ret;

  s = *ps;
  while(s < es && strchr(whitespace, *s))
    s++;
  if(q)
    *q = s;
  ret = *s;
  switch(*s){
  case 0:
    break;
  case '|':
  case '(':
  case ')':
  case ';':
  case '&':
  case '<':
    s++;
    break;
  case '>':
    s++;
    if(*s == '>'){
      ret = '+';
      s++;
    }
    break;
  default:
    ret = 'a';
    while(s < es && !strchr(whitespace, *s) && !strchr(symbols, *s))
      s++;
    break;
  }
  if(eq)
    *eq = s;

  while(s < es && strchr(whitespace, *s))
    s++;
  *ps = s;
  return ret;
}

int
peek(char **ps, char *es, char *toks)
{
  char *s;

  s = *ps;
  while(s < es && strchr(whitespace, *s))
    s++;
  *ps = s;
  return *s && strchr(toks, *s);
}

struct cmd *parseline(char**, char*);
struct cmd *parsepipe(char**, char*);
struct cmd *parseexec(char**, char*);
struct cmd *nulterminate(struct cmd*);

struct cmd*
parsecmd(char *s)
{
  char *es;
  struct cmd *cmd;

  es = s + strlen(s);
  cmd = parseline(&s, es);
  peek(&s, es, "");
  if(s != es){
    printf(2, "leftovers: %s\n", s);
    panic("syntax");
  }
  nulterminate(cmd);
  return cmd;
}

struct cmd*
parseline(char **ps, char *es)
{
  struct cmd *cmd;

  cmd = parsepipe(ps, es);
  while(peek(ps, es, "&")){
    gettoken(ps, es, 0, 0);
    cmd = backcmd(cmd);
  }
  if(peek(ps, es, ";")){
    gettoken(ps, es, 0, 0);
    cmd = listcmd(cmd, parseline(ps, es));
  }
  return cmd;
}

struct cmd*
parsepipe(char **ps, char *es)
{
  struct cmd *cmd;

  cmd = parseexec(ps, es);
  if(peek(ps, es, "|")){
    gettoken(ps, es, 0, 0);
    cmd = pipecmd(cmd, parsepipe(ps, es));
  }
  return cmd;
}

struct cmd*
parseredirs(struct cmd *cmd, char **ps, char *es)
{
  int tok;
  char *q, *eq;

  while(peek(ps, es, "<>")){
    tok = gettoken(ps, es, 0, 0);
    if(gettoken(ps, es, &q, &eq) != 'a')
      panic("missing file for redirection");
    switch(tok){
    case '<':
      cmd = redircmd(cmd, q, eq, O_RDONLY, 0);
      break;
    case '>':
      cmd = redircmd(cmd, q, eq, O_WRONLY|O_CREATE, 1);
      break;
    case '+':  // >>
      cmd = redircmd(cmd, q, eq, O_WRONLY|O_CREATE, 1);
      break;
    }
  }
  return cmd;
}

struct cmd*
parseblock(char **ps, char *es)
{
  struct cmd *cmd;

  if(!peek(ps, es, "("))
    panic("parseblock");
  gettoken(ps, es, 0, 0);
  cmd = parseline(ps, es);
  if(!peek(ps, es, ")"))
    panic("syntax - missing )");
  gettoken(ps, es, 0, 0);
  cmd = parseredirs(cmd, ps, es);
  return cmd;
}

struct cmd*
parseexec(char **ps, char *es)
{
  char *q, *eq;
  int tok, argc;
  struct execcmd *cmd;
  struct cmd *ret;

  if(peek(ps, es, "("))
    return parseblock(ps, es);

  ret = execcmd();
  cmd = (struct execcmd*)ret;

  argc = 0;
  ret = parseredirs(ret, ps, es);
  while(!peek(ps, es, "|)&;")){
    if((tok=gettoken(ps, es, &q, &eq)) == 0)
      break;
    if(tok != 'a')
      panic("syntax");
    cmd->argv[argc] = q;
    cmd->eargv[argc] = eq;
    argc++;
    if(argc >= MAXARGS)
      panic("too many args");
    ret = parseredirs(ret, ps, es);
  }
  cmd->argv[argc] = 0;
  cmd->eargv[argc] = 0;
  return ret;
}

// NUL-terminate all the counted strings.
struct cmd*
nulterminate(struct cmd *cmd)
{
  int i;
  struct backcmd *bcmd;
  struct execcmd *ecmd;
  struct listcmd *lcmd;
  struct pipecmd *pcmd;
  struct redircmd *rcmd;

  if(cmd == 0)
    return 0;

  switch(cmd->type){
  case EXEC:
    ecmd = (struct execcmd*)cmd;
    for(i=0; ecmd->argv[i]; i++)
      *ecmd->eargv[i] = 0;
    break;

  case REDIR:
    rcmd = (struct redircmd*)cmd;
    nulterminate(rcmd->cmd);
    *rcmd->efile = 0;
    break;

  case PIPE:
    pcmd = (struct pipecmd*)cmd;
    nulterminate(pcmd->left);
    nulterminate(pcmd->right);
    break;

  case LIST:
    lcmd = (struct listcmd*)cmd;
    nulterminate(lcmd->left);
    nulterminate(lcmd->right);
    break;

  case BACK:
    bcmd = (struct backcmd*)cmd;
    nulterminate(bcmd->cmd);
    break;
  }
  return cmd;
}
