// Shell.
// 参考资料
// https://wjqwsp.github.io/2017/06/04/xv6-shell%E5%AE%9E%E7%8E%B0%E6%BA%90%E4%BB%A3%E7%A0%81%E5%88%86%E6%9E%90/

#include "types.h"
#include "user.h"
#include "fcntl.h"

// Parsed command representation
#define EXEC  1
#define REDIR 2
#define PIPE  3
#define LIST  4
#define BACK  5

#define MAXARGS 10

// 命令接口：借鉴了面向对象思想
struct cmd {
  int type;
};

// 普通命令
struct execcmd {
  int type;  // EXEC 1
  char *argv[MAXARGS];  // 字符指针数组，每个指针指向一个参数字符串的开始地址
  char *eargv[MAXARGS]; // 字符指针数组，每个指针指向一个参数字符串的结束地址
};

// 重定向命令
struct redircmd {
  int type;  // REDIR 2
  struct cmd *cmd;  // 实际执行的命令
  char *file;  // 重定向文件名起始地址
  char *efile;  // 重定向文件名结束地址
  int mode;  // 读写模式:只读、只写、读写
  int fd;  // 重定向替换的文件描述符（标准输入、标准输出等）
};

// 管道命令
struct pipecmd {
  int type;  // PIPE 3
  struct cmd *left;  // 管道输入命令
  struct cmd *right;  // 管道输出命令
};

// 并列命令,一行包含两个命令，以;分割
struct listcmd {
  int type;  // LIST 4
  struct cmd *left;  // 第一个命令
  struct cmd *right;  // 第二个命令
};

// 后台命令, 命令结尾加了&的命令
struct backcmd {
  int type;  // BACK 5
  struct cmd *cmd;  // 命令
};

int fork1(void);  // Fork but panics on failure.
void panic(char*);
struct cmd *parsecmd(char*);

// Execute cmd.  Never returns.
void
runcmd(struct cmd *cmd)
{
  int p[2];
  struct backcmd *bcmd;
  struct execcmd *ecmd;
  struct listcmd *lcmd;
  struct pipecmd *pcmd;
  struct redircmd *rcmd;

  if(cmd == 0)
    exit();

  switch(cmd->type){
  default:
    panic("runcmd");

  case EXEC:
    ecmd = (struct execcmd*)cmd;
    if(ecmd->argv[0] == 0)
      exit();
    exec(ecmd->argv[0], ecmd->argv);  // exec函数会取代执行它的进程,也就是说,一旦exec函数执行成功,它就不会返回了,进程结束.但是如果exec函数执行失败,它会返回失败的信息,而且进程继续执行后面的代码
    printf(2, "exec %s failed\n", ecmd->argv[0]);
    break;

  case REDIR:
    rcmd = (struct redircmd*)cmd;
    // 重定向，关闭要重定向的文件描述符，然后open新的文件占用之前的0,1,2
    close(rcmd->fd);  // close(http://man7.org/linux/man-pages/man2/close.2.html)
    if(open(rcmd->file, rcmd->mode) < 0){
      printf(2, "open %s failed\n", rcmd->file);
      exit();
    }
    runcmd(rcmd->cmd);
    break;

  case LIST:
    lcmd = (struct listcmd*)cmd;
    if(fork1() == 0)
      runcmd(lcmd->left);
    wait();
    runcmd(lcmd->right);
    break;

  case PIPE:
    pcmd = (struct pipecmd*)cmd;
    if(pipe(p) < 0) // pipe(http://man7.org/linux/man-pages/man7/pipe.7.html)
      panic("pipe");
    if(fork1() == 0){
      close(1);
      dup(p[1]); // dup(http://man7.org/linux/man-pages/man2/dup.2.html) 复制pipe的write端句柄给标准输出(1)
      // 子进程关闭管道读写描述符
      close(p[0]);
      close(p[1]);
      runcmd(pcmd->left);
    }
    if(fork1() == 0){
      close(0);
      dup(p[0]);  // 复制pipe的read端句柄给标准输入(0)
      close(p[0]);
      close(p[1]);
      runcmd(pcmd->right);
    }
    // 父进程关闭管道文件描述符
    close(p[0]);
    close(p[1]);
    wait();
    wait();
    break;

  case BACK:
    bcmd = (struct backcmd*)cmd;
    if(fork1() == 0)
      runcmd(bcmd->cmd);
    break;
  }
  exit();
}

// 从标准输入读取一行命令
// success 0
// fail -1
int
getcmd(char *buf, int nbuf)
{
  printf(2, "$ ");
  memset(buf, 0, nbuf);  // 初始化数组buf memset(http://man7.org/linux/man-pages/man3/memset.3.html)
  gets(buf, nbuf);  // 从标准输入读取一行到buf中 gets(http://man7.org/linux/man-pages/man3/gets.3.html)
  if(buf[0] == 0) // EOF
    return -1;
  return 0;
}

int
main(void)
{
  static char buf[100];
  int fd;

  // Ensure that three file descriptors are open.
  // console指/dev/console
  // fd文件描述符是非负整数，并且linux是从小到大分配的。其中0,1,2分别指标准输入,标准输出,标准错误
  // 参考资料
  // https://unix.stackexchange.com/questions/72000/open-console-for-default-file-descriptors
  // http://man7.org/linux/man-pages/man2/open.2.html
  while((fd = open("console", O_RDWR)) >= 0){
    if(fd >= 3){
      close(fd);
      break;
    }
  }

  // Read and run input commands.
  while(getcmd(buf, sizeof(buf)) >= 0){
    if(buf[0] == 'c' && buf[1] == 'd' && buf[2] == ' '){
      // Chdir must be called by the parent, not the child.
      buf[strlen(buf)-1] = 0;  // chop \n  去掉'\n'
      if(chdir(buf+3) < 0)  // chdir(http://pubs.opengroup.org/onlinepubs/009695399/functions/chdir.html) 更改工作目录
        printf(2, "cannot cd %s\n", buf+3);
      continue;
    }
    if(fork1() == 0)
        runcmd(parsecmd(buf));
    // wait(http://man7.org/linux/man-pages/man2/waitpid.2.html)
    // wait函数来回收子进程的资源,如果一个子进程未被父进程执行wait，则会成为“僵尸进程”，在父进程exit后会被init进程(1号进程)收养,然后1号进程会执行wait()操作
    // 内核会维护僵尸进程的最小信息集（PID、退出状态、资源使用统计信息）
    // 僵尸进程会占用内核的进程表
    wait();
  }
  // exit(http://man7.org/linux/man-pages/man3/exit.3.html)
  // 执行exit后，大部分占用的资源都会释放，除了最小信息集，等待父进程wait()
  exit();
}

void
panic(char *s)
{
  printf(2, "%s\n", s);
  exit();
}

int
fork1(void)
{
  int pid;

  pid = fork();  // fork(http://man7.org/linux/man-pages/man2/fork.2.html)
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

// 词法提取：提取命令或文件名,过滤掉symbols和whitespace
// ps 命令开头
// es 命令结尾
// q 提取的字符的开始地址
// eq提取的字符的结束地址
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
  // 过滤掉命令开头的空格等空字符
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
  cmd = parseline(&s, es);  // 解析具体命令
  peek(&s, es, "");  // 从头开始过滤空字符
  if(s != es){  // 命令仍有多余字符，报语法错误
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
  while(peek(ps, es, "&")){ // 处理后台命令
    gettoken(ps, es, 0, 0);
    cmd = backcmd(cmd);
  }
  if(peek(ps, es, ";")){  // 处理并行命令
    gettoken(ps, es, 0, 0);
    cmd = listcmd(cmd, parseline(ps, es));
  }
  return cmd;
}

// 解析管道命令
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

// 解析重定向命令
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

// 过滤掉()
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

// 构造基本命令
// 字符串开头地址
// 字符串结尾地址
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
// 将命令结构中的每个字符串结尾都替换为\0
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
